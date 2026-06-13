#!/usr/bin/env node
/**
 * Phase 1 -- Graph Topology Analysis Script
 * Analyzes the assembled knowledge graph to compute structural properties
 * used by the Phase 2 tour builder.
 */

const fs = require('fs');
const path = require('path');

// --- Parse arguments ---
const inputPath = process.argv[2];
const outputPath = process.argv[3];

if (!inputPath || !outputPath) {
  console.error('Usage: node ua-tour-analyze.js <input.json> <output.json>');
  process.exit(1);
}

let data;
try {
  data = JSON.parse(fs.readFileSync(inputPath, 'utf-8'));
} catch (e) {
  console.error('Failed to read input file:', e.message);
  process.exit(1);
}

const { nodes, edges, layers } = data;

// Build lookup maps
const nodeMap = {};
for (const n of nodes) {
  nodeMap[n.id] = n;
}

// --- A. Fan-In Ranking ---
// Count how many edges point TO each node (considering only file-to-file and class-level edges)
const fanIn = {};
for (const n of nodes) {
  fanIn[n.id] = 0;
}
for (const e of edges) {
  if (e.target && fanIn.hasOwnProperty(e.target)) {
    fanIn[e.target]++;
  }
}
const fanInRanking = Object.entries(fanIn)
  .filter(([id]) => nodeMap[id] && nodeMap[id].type === 'file')
  .map(([id, count]) => ({ id, fanIn: count, name: nodeMap[id].name }))
  .sort((a, b) => b.fanIn - a.fanIn)
  .slice(0, 20);

// --- B. Fan-Out Ranking ---
const fanOut = {};
for (const n of nodes) {
  fanOut[n.id] = 0;
}
for (const e of edges) {
  if (e.source && fanOut.hasOwnProperty(e.source)) {
    fanOut[e.source]++;
  }
}
const fanOutRanking = Object.entries(fanOut)
  .filter(([id]) => nodeMap[id] && nodeMap[id].type === 'file')
  .map(([id, count]) => ({ id, fanOut: count, name: nodeMap[id].name }))
  .sort((a, b) => b.fanOut - a.fanOut)
  .slice(0, 20);

// --- C. Entry Point Candidates ---
const entryPointFileNamePatterns = [
  'index.ts', 'index.js', 'main.ts', 'main.js', 'app.ts', 'app.js',
  'server.ts', 'server.js', 'mod.rs', 'main.go', 'main.py', 'main.rs',
  'manage.py', 'app.py', 'wsgi.py', 'asgi.py', 'run.py', '__main__.py',
  'Application.java', 'Main.java', 'Program.cs', 'config.ru', 'index.php',
  'App.swift', 'Application.kt', 'main.cpp', 'main.c'
];

// Compute top 10% fan-out threshold among file nodes
const fileFanOuts = fanOutRanking.map(f => f.fanOut);
const top10Threshold = fileFanOuts.length > 0
  ? fileFanOuts[Math.floor(fileFanOuts.length * 0.1)]
  : 0;

// Compute bottom 25% fan-in threshold
const fileFanIns = Object.values(fanIn)
  .filter((_, id) => nodeMap[id] && nodeMap[id].type === 'file');
const sortedFanIns = [...fileFanIns].sort((a, b) => a - b);
const bottom25Threshold = sortedFanIns.length > 0
  ? sortedFanIns[Math.floor(sortedFanIns.length * 0.25)]
  : 0;

const candidates = [];
for (const n of nodes) {
  let score = 0;
  const fileName = n.name || '';
  const filePath = n.filePath || '';

  if (n.type === 'file') {
    // Filename matches entry point patterns
    if (entryPointFileNamePatterns.some(p => fileName === p)) {
      score += 3;
    }
    // File is at project root or one level deep
    const depth = filePath.split('/').filter(Boolean).length;
    if (depth <= 2 && !filePath.startsWith('ui/') && !filePath.startsWith('include/') && !filePath.startsWith('src/')) {
      // root level
      score += 1;
    } else if (/^src\/[^/]+$/.test(filePath) || /^app\/[^/]+$/.test(filePath)) {
      score += 1;
    }
    // High fan-out (top 10%)
    if ((fanOut[n.id] || 0) >= top10Threshold && top10Threshold > 0) {
      score += 1;
    }
    // Low fan-in (bottom 25%)
    if ((fanIn[n.id] || 0) <= bottom25Threshold) {
      score += 1;
    }
  } else if (n.type === 'document') {
    // README.md at project root
    if (fileName === 'README.md' && !filePath.includes('/')) {
      score += 5;
    }
    // Other .md at project root
    if (fileName.endsWith('.md') && !filePath.includes('/')) {
      score += 2;
    }
  }

  if (score > 0) {
    candidates.push({
      id: n.id,
      score,
      name: n.name,
      summary: n.summary || ''
    });
  }
}
candidates.sort((a, b) => b.score - a.score);
const entryPointCandidates = candidates.slice(0, 5);

// --- D. BFS Traversal from top code entry point ---
// Find the top code entry point (file type, not document)
const topCodeEntry = entryPointCandidates.find(c => nodeMap[c.id] && nodeMap[c.id].type === 'file');
const bfsStartNode = topCodeEntry ? topCodeEntry.id : null;

const bfsTraversal = { startNode: bfsStartNode, order: [], depthMap: {}, byDepth: {} };

if (bfsStartNode) {
  // Build adjacency list for imports and calls edges (forward direction only, for file-to-file edges)
  const adjacency = {};
  for (const n of nodes) {
    if (n.type === 'file') {
      adjacency[n.id] = [];
    }
  }
  for (const e of edges) {
    if (e.type === 'imports' || e.type === 'calls' || e.type === 'depends_on') {
      if (adjacency.hasOwnProperty(e.source) && adjacency.hasOwnProperty(e.target)) {
        adjacency[e.source].push(e.target);
      }
    }
  }

  // BFS
  const visited = new Set();
  const queue = [bfsStartNode];
  const depth = {};
  depth[bfsStartNode] = 0;
  visited.add(bfsStartNode);

  while (queue.length > 0) {
    const current = queue.shift();
    bfsTraversal.order.push(current);
    const currentDepth = depth[current];

    const neighbors = adjacency[current] || [];
    for (const neighbor of neighbors) {
      if (!visited.has(neighbor)) {
        visited.add(neighbor);
        depth[neighbor] = currentDepth + 1;
        queue.push(neighbor);
      }
    }
  }

  bfsTraversal.depthMap = depth;
  // Group by depth
  const byDepth = {};
  for (const [nodeId, d] of Object.entries(depth)) {
    if (!byDepth[d]) byDepth[d] = [];
    byDepth[d].push(nodeId);
  }
  bfsTraversal.byDepth = byDepth;
}

// --- E. Non-Code File Inventory ---
const nonCodeFiles = {
  documentation: [],
  infrastructure: [],
  data: [],
  config: []
};

for (const n of nodes) {
  const entry = { id: n.id, name: n.name, summary: n.summary || '' };
  switch (n.type) {
    case 'document':
      nonCodeFiles.documentation.push(entry);
      break;
    case 'service':
    case 'pipeline':
    case 'resource':
      nonCodeFiles.infrastructure.push(entry);
      break;
    case 'table':
    case 'schema':
    case 'endpoint':
      nonCodeFiles.data.push(entry);
      break;
    case 'config':
      nonCodeFiles.config.push(entry);
      break;
  }
}

// --- F. Tightly Coupled Clusters ---
// Find pairs with bidirectional relationships, then expand clusters
const bidirectionalPairs = [];
const edgeSet = new Set();
for (const e of edges) {
  if (e.type === 'imports' || e.type === 'calls' || e.type === 'depends_on') {
    if (nodeMap[e.source] && nodeMap[e.source].type === 'file' &&
        nodeMap[e.target] && nodeMap[e.target].type === 'file') {
      edgeSet.add(`${e.source}|||${e.target}`);
    }
  }
}

for (const [sourceId, targetId] of [...edgeSet].map(s => s.split('|||'))) {
  if (sourceId === targetId) continue;
  const reverseKey = `${targetId}|||${sourceId}`;
  if (edgeSet.has(reverseKey)) {
    // Check we haven't already added this pair
    const alreadyAdded = bidirectionalPairs.some(
      p => (p[0] === sourceId && p[1] === targetId) || (p[0] === targetId && p[1] === sourceId)
    );
    if (!alreadyAdded) {
      bidirectionalPairs.push([sourceId, targetId]);
    }
  }
}

// Build clusters from bidirectional pairs
const clusters = [];
const clusterNodes = new Set();

for (const pair of bidirectionalPairs) {
  const [a, b] = pair;
  // Find existing cluster containing either node
  let existingCluster = null;
  for (const c of clusters) {
    if (c.nodes.has(a) || c.nodes.has(b)) {
      existingCluster = c;
      break;
    }
  }
  if (existingCluster) {
    existingCluster.nodes.add(a);
    existingCluster.nodes.add(b);
  } else {
    const newSet = new Set([a, b]);
    clusters.push({ nodes: newSet, edgeCount: 2 });
  }
}

// Expand clusters: add nodes that connect to 2+ existing members
const allFileNodes = nodes.filter(n => n.type === 'file').map(n => n.id);
let expanded = true;
while (expanded) {
  expanded = false;
  for (const nodeId of allFileNodes) {
    if (clusterNodes.has(nodeId)) continue;
    for (const c of clusters) {
      let connections = 0;
      for (const member of c.nodes) {
        const key1 = `${nodeId}|||${member}`;
        const key2 = `${member}|||${nodeId}`;
        if (edgeSet.has(key1) || edgeSet.has(key2)) {
          connections++;
        }
      }
      if (connections >= 2) {
        c.nodes.add(nodeId);
        clusterNodes.add(nodeId);
        expanded = true;
        break;
      }
    }
  }
  // Update clusterNodes
  for (const c of clusters) {
    for (const n of c.nodes) {
      clusterNodes.add(n);
    }
  }
}

const clusterList = clusters
  .filter(c => c.nodes.size >= 2 && c.nodes.size <= 5)
  .sort((a, b) => b.nodes.size - a.nodes.size)
  .slice(0, 10)
  .map(c => ({
    nodes: [...c.nodes],
    edgeCount: c.edgeCount
  }));

// --- G. Layer List ---
const layerInfo = {
  count: layers ? layers.length : 0,
  list: layers ? layers.map(l => ({ id: l.id, name: l.name, description: l.description })) : []
};

// --- H. Node Summary Index ---
const nodeSummaryIndex = {};
for (const n of nodes) {
  nodeSummaryIndex[n.id] = {
    name: n.name || '',
    type: n.type || '',
    summary: n.summary || ''
  };
}

// --- Output ---
const result = {
  scriptCompleted: true,
  entryPointCandidates,
  fanInRanking,
  fanOutRanking,
  bfsTraversal,
  nonCodeFiles,
  clusters: clusterList,
  layers: layerInfo,
  nodeSummaryIndex,
  totalNodes: nodes.length,
  totalEdges: edges.length
};

try {
  fs.mkdirSync(path.dirname(outputPath), { recursive: true });
  fs.writeFileSync(outputPath, JSON.stringify(result, null, 2), 'utf-8');
  console.log(`Analysis complete. ${nodes.length} nodes, ${edges.length} edges.`);
  console.log(`Top entry: ${entryPointCandidates.length > 0 ? entryPointCandidates[0].name : 'none'}`);
  console.log(`BFS reached ${bfsTraversal.order.length} nodes from ${bfsStartNode || 'none'}`);
  console.log(`Clusters found: ${clusterList.length}`);
  console.log(`Output: ${outputPath}`);
  process.exit(0);
} catch (e) {
  console.error('Failed to write output:', e.message);
  process.exit(1);
}

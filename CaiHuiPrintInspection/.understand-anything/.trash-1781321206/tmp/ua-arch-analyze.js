#!/usr/bin/env node
/**
 * Structural Architecture Analyzer for Understand-Anything
 * Reads assembled-graph.json and produces ua-arch-results.json
 */
const fs = require('fs');
const path = require('path');

const inputPath = process.argv[2];
const outputPath = process.argv[3];

if (!inputPath || !outputPath) {
  console.error('Usage: node ua-arch-analyze.js <assembled-graph.json> <output.json>');
  process.exit(1);
}

try {
  const raw = fs.readFileSync(inputPath, 'utf-8');
  const graph = JSON.parse(raw);

  const allNodes = graph.nodes || [];
  const allEdges = graph.edges || [];

  // === Step 1: Identify file-level nodes (not class/function sub-file nodes) ===
  const fileNodeIds = new Set();
  const fileNodes = [];
  const nonFileNodes = [];

  for (const node of allNodes) {
    if (node.type === 'file' || node.type === 'document' || node.type === 'config' ||
        node.type === 'service' || node.type === 'pipeline' || node.type === 'table' ||
        node.type === 'schema' || node.type === 'resource' || node.type === 'endpoint') {
      fileNodeIds.add(node.id);
      fileNodes.push(node);
    } else if (node.type === 'class' || node.type === 'function' || node.type === 'method' ||
               node.type === 'interface' || node.type === 'variable' || node.type === 'type') {
      nonFileNodes.push(node);
      // Also include them in fileNodeIds if we want them for directory grouping
      // But we only want file-level nodes for layers
    }
  }

  // === Step 2: Extract import edges between file-level nodes ===
  const importEdges = [];
  const configuresEdges = [];
  const deploysEdges = [];
  const definesSchemaEdges = [];
  const documentsEdges = [];
  const dependsOnEdges = [];
  const callsEdges = [];
  const relatedEdges = [];
  const containsEdges = [];
  const exportsEdges = [];
  const inheritsEdges = [];

  for (const edge of allEdges) {
    switch (edge.type) {
      case 'imports':
        if (fileNodeIds.has(edge.source) && fileNodeIds.has(edge.target)) {
          importEdges.push(edge);
        }
        break;
      case 'configures':
        configuresEdges.push(edge);
        break;
      case 'deploys':
        deploysEdges.push(edge);
        break;
      case 'defines_schema':
        definesSchemaEdges.push(edge);
        break;
      case 'documents':
        documentsEdges.push(edge);
        break;
      case 'depends_on':
        dependsOnEdges.push(edge);
        break;
      case 'calls':
        callsEdges.push(edge);
        break;
      case 'related':
        relatedEdges.push(edge);
        break;
      case 'contains':
        containsEdges.push(edge);
        break;
      case 'exports':
        exportsEdges.push(edge);
        break;
      case 'inherits':
        inheritsEdges.push(edge);
        break;
    }
  }

  // === A. Directory Grouping ===
  // Find common prefix among all files
  function findCommonPrefix(paths) {
    if (paths.length === 0) return '';
    const parts = paths.map(p => p.split('/'));
    const minLen = Math.min(...parts.map(p => p.length));
    let common = [];
    for (let i = 0; i < minLen; i++) {
      const first = parts[0][i];
      if (parts.every(p => p[i] === first)) {
        common.push(first);
      } else {
        break;
      }
    }
    return common.length > 0 ? common.join('/') + '/' : '';
  }

  const allFilePaths = fileNodes.map(n => n.filePath);
  const commonPrefix = findCommonPrefix(allFilePaths);

  // Group by first directory segment after common prefix
  const directoryGroups = {};
  fileNodes.forEach(node => {
    let relPath = node.filePath;
    if (commonPrefix && relPath.startsWith(commonPrefix)) {
      relPath = relPath.substring(commonPrefix.length);
    }
    const parts = relPath.split('/');
    let groupKey;
    if (parts.length > 1) {
      groupKey = parts[0];
    } else {
      // Root-level file
      groupKey = '(root)';
    }

    // Special handling: sometimes files have no common prefix, group by first segment
    if (groupKey === '(root)' && commonPrefix === '') {
      const absParts = node.filePath.split('/');
      groupKey = absParts[0] || '(root)';
    }

    if (!directoryGroups[groupKey]) {
      directoryGroups[groupKey] = [];
    }
    directoryGroups[groupKey].push(node.id);
  });

  // === B. Node Type Grouping ===
  const nodeTypeGroups = {};
  fileNodes.forEach(node => {
    if (!nodeTypeGroups[node.type]) {
      nodeTypeGroups[node.type] = [];
    }
    nodeTypeGroups[node.type].push(node.id);
  });

  // Also include non-file nodes for type grouping if needed
  // But we only need file-level for layers

  // === C. Import Adjacency Matrix ===
  const fanOut = {};
  const fanIn = {};
  fileNodes.forEach(n => {
    fanOut[n.id] = 0;
    fanIn[n.id] = 0;
  });

  importEdges.forEach(e => {
    fanOut[e.source] = (fanOut[e.source] || 0) + 1;
    fanIn[e.target] = (fanIn[e.target] || 0) + 1;
  });

  // Group-level imports
  const groupToNode = {};
  for (const [group, ids] of Object.entries(directoryGroups)) {
    ids.forEach(id => { groupToNode[id] = group; });
  }

  const groupImportCounts = {};
  importEdges.forEach(e => {
    const fromGroup = groupToNode[e.source];
    const toGroup = groupToNode[e.target];
    if (fromGroup && toGroup && fromGroup !== toGroup) {
      const key = `${fromGroup}->${toGroup}`;
      groupImportCounts[key] = (groupImportCounts[key] || 0) + 1;
    }
  });

  // === D. Cross-Category Dependency Analysis ===
  const crossCategoryEdges = [];
  const crossCatMap = {};

  // Collect all file-level edges (not just imports)
  const allFileLevelEdges = allEdges.filter(e => {
    // At least one end is file-level
    return fileNodeIds.has(e.source) || fileNodeIds.has(e.target);
  });

  allFileLevelEdges.forEach(e => {
    const srcNode = allNodes.find(n => n.id === e.source);
    const tgtNode = allNodes.find(n => n.id === e.target);
    if (!srcNode || !tgtNode) return;
    // Only count edges between file-level nodes or from non-file to file
    const fromType = fileNodeIds.has(e.source) ? srcNode.type : srcNode.type;
    const toType = fileNodeIds.has(e.target) ? tgtNode.type : tgtNode.type;
    const key = `${fromType}|${toType}|${e.type}`;
    crossCatMap[key] = (crossCatMap[key] || 0) + 1;
  });

  for (const [key, count] of Object.entries(crossCatMap)) {
    const [fromType, toType, edgeType] = key.split('|');
    crossCategoryEdges.push({ fromType, toType, edgeType, count });
  }

  // === E. Inter-Group Import Frequency ===
  const interGroupImports = [];
  for (const [key, count] of Object.entries(groupImportCounts)) {
    const [from, to] = key.split('->');
    interGroupImports.push({ from, to, count });
  }
  interGroupImports.sort((a, b) => b.count - a.count);

  // === F. Intra-Group Import Density ===
  const intraGroupDensity = {};
  for (const [group, ids] of Object.entries(directoryGroups)) {
    const idSet = new Set(ids);
    let internalEdges = 0;
    let totalEdges = 0;
    importEdges.forEach(e => {
      if (idSet.has(e.source) && idSet.has(e.target)) {
        totalEdges++;
        if (group === groupToNode[e.source] && group === groupToNode[e.target]) {
          internalEdges++;
        }
      }
    });
    // Also count depends_on and calls edges for total
    dependsOnEdges.forEach(e => {
      if (idSet.has(e.source) && idSet.has(e.target)) totalEdges++;
    });
    callsEdges.forEach(e => {
      if (idSet.has(e.source) && idSet.has(e.target)) totalEdges++;
    });

    intraGroupDensity[group] = {
      internalEdges,
      totalEdges: totalEdges || 1,
      density: totalEdges > 0 ? internalEdges / totalEdges : 0
    };
  }

  // === G. Directory Pattern Matching ===
  const patternMap = {
    'routes': 'api', 'api': 'api', 'controllers': 'api', 'endpoints': 'api',
    'handlers': 'api', 'blueprints': 'api', 'routers': 'api', 'serializers': 'api',
    'services': 'service', 'core': 'service', 'lib': 'service', 'domain': 'service',
    'logic': 'service', 'signals': 'service', 'internal': 'service', 'composables': 'service',
    'mailers': 'service', 'jobs': 'service', 'channels': 'service',
    'models': 'data', 'db': 'data', 'data': 'data', 'persistence': 'data',
    'repository': 'data', 'entities': 'data', 'entity': 'data', 'migrations': 'data',
    'sql': 'data', 'database': 'data', 'schema': 'data',
    'components': 'ui', 'views': 'ui', 'pages': 'ui', 'ui': 'ui',
    'layouts': 'ui', 'screens': 'ui',
    'middleware': 'middleware', 'plugins': 'middleware', 'interceptors': 'middleware',
    'guards': 'middleware',
    'utils': 'utility', 'helpers': 'utility', 'common': 'utility', 'shared': 'utility',
    'tools': 'utility', 'pkg': 'utility', 'templatetags': 'utility',
    'config': 'config', 'constants': 'config', 'env': 'config', 'settings': 'config',
    'management': 'config', 'commands': 'config',
    'test': 'test', 'tests': 'test', 'spec': 'test', 'specs': 'test',
    '__tests__': 'test',
    'types': 'types', 'interfaces': 'types', 'schemas': 'types', 'contracts': 'types',
    'dtos': 'types', 'dto': 'types', 'request': 'types', 'response': 'types',
    'hooks': 'hooks',
    'store': 'state', 'state': 'state', 'reducers': 'state', 'actions': 'state',
    'slices': 'state',
    'assets': 'assets', 'static': 'assets', 'public': 'assets',
    'cmd': 'entry', 'bin': 'entry',
    'docs': 'documentation', 'documentation': 'documentation', 'wiki': 'documentation',
    'deploy': 'infrastructure', 'deployment': 'infrastructure', 'infra': 'infrastructure',
    'infrastructure': 'infrastructure', 'k8s': 'infrastructure', 'kubernetes': 'infrastructure',
    'helm': 'infrastructure', 'charts': 'infrastructure', 'terraform': 'infrastructure',
    'tf': 'infrastructure', 'docker': 'infrastructure',
    '.github': 'ci-cd', '.gitlab': 'ci-cd', '.circleci': 'ci-cd',
    'Modules': 'service',
    'DetachThread': 'service',
    'func': 'service',
    'NoHardWare': 'test',
    'include': 'service',
    'src': 'entry',
    'osoFile': 'data',
    'osoInclude': 'data',
    'qrc': 'assets',
  };

  const patternMatches = {};
  for (const group of Object.keys(directoryGroups)) {
    const lower = group.toLowerCase();
    patternMatches[group] = patternMap[lower] || patternMap[group] || 'unknown';
  }

  // Also check file-level patterns
  fileNodes.forEach(node => {
    const name = node.name;
    const fp = node.filePath;
    // .oso files are schema definitions
    if (name.endsWith('.oso')) {
      // Already handled by directory
    }
  });

  // === H. Deployment Topology Detection ===
  const deploymentTopology = {
    hasDockerfile: false,
    hasCompose: false,
    hasK8s: false,
    hasTerraform: false,
    hasCI: false,
    infraFiles: []
  };

  fileNodes.forEach(node => {
    const name = node.name.toLowerCase();
    const fp = node.filePath.toLowerCase();
    if (name === 'dockerfile' || name.startsWith('dockerfile.')) {
      deploymentTopology.hasDockerfile = true;
      deploymentTopology.infraFiles.push(node.filePath);
    }
    if (name.startsWith('docker-compose')) {
      deploymentTopology.hasCompose = true;
      deploymentTopology.infraFiles.push(node.filePath);
    }
    if (fp.includes('.github/workflows') || fp.includes('.gitlab-ci') ||
        name === 'jenkinsfile') {
      deploymentTopology.hasCI = true;
      deploymentTopology.infraFiles.push(node.filePath);
    }
    if (name.endsWith('.tf') || name.endsWith('.tfvars')) {
      deploymentTopology.hasTerraform = true;
      deploymentTopology.infraFiles.push(node.filePath);
    }
  });

  // === I. Data Pipeline Detection ===
  const dataPipeline = {
    schemaFiles: [],
    migrationFiles: [],
    dataModelFiles: [],
    apiHandlerFiles: []
  };

  fileNodes.forEach(node => {
    const fp = node.filePath.toLowerCase();
    const name = node.name;
    if (name.endsWith('.oso') || name.endsWith('.graphql') || name.endsWith('.gql') ||
        name.endsWith('.proto') || name.endsWith('.prisma')) {
      dataPipeline.schemaFiles.push(node.filePath);
    }
    if (fp.includes('migration') && name.endsWith('.sql')) {
      dataPipeline.migrationFiles.push(node.filePath);
    }
    if (fp.includes('/data/') || fp.includes('/models/') || fp.includes('/entities/') ||
        fp.includes('halcondata') || fp.includes('utilty') || fp.includes('osoinclude')) {
      dataPipeline.dataModelFiles.push(node.filePath);
    }
    if (fp.includes('/routes/') || fp.includes('/api/') || fp.includes('/controllers/') ||
        fp.includes('/handlers/') || fp.includes('/endpoints/') ||
        node.tags.includes('api-handler')) {
      dataPipeline.apiHandlerFiles.push(node.filePath);
    }
  });

  // === J. Documentation Coverage ===
  const docCoverage = {
    groupsWithDocs: 0,
    totalGroups: Object.keys(directoryGroups).length,
    coverageRatio: 0,
    undocumentedGroups: []
  };

  for (const [group, ids] of Object.entries(directoryGroups)) {
    const hasDoc = ids.some(id => {
      const node = fileNodes.find(n => n.id === id);
      return node && node.type === 'document';
    });
    if (hasDoc) {
      docCoverage.groupsWithDocs++;
    } else {
      // Check if any document file references this group
      const docFiles = fileNodes.filter(n => n.type === 'document');
      const hasRef = docFiles.some(doc => {
        const edges = allEdges.filter(e =>
          e.source === doc.id && ids.includes(e.target)
        );
        return edges.length > 0;
      });
      if (!hasRef) {
        docCoverage.undocumentedGroups.push(group);
      } else {
        docCoverage.groupsWithDocs++;
      }
    }
  }
  docCoverage.coverageRatio = docCoverage.totalGroups > 0 ?
    docCoverage.groupsWithDocs / docCoverage.totalGroups : 0;

  // === K. Dependency Direction ===
  const dependencyDirection = [];
  const processed = new Set();
  for (const [key, count] of Object.entries(groupImportCounts)) {
    const [from, to] = key.split('->');
    const reverseKey = `${to}->${from}`;
    if (processed.has(key)) continue;
    processed.add(key);
    processed.add(reverseKey);
    const reverseCount = groupImportCounts[reverseKey] || 0;
    if (count > reverseCount) {
      dependencyDirection.push({ dependent: from, dependsOn: to });
    } else if (reverseCount > count) {
      dependencyDirection.push({ dependent: to, dependsOn: from });
    }
    // If equal, skip (bidirectional)
  }

  // === File Stats ===
  const fileStats = {
    totalFileNodes: fileNodes.length,
    filesPerGroup: {},
    nodeTypeCounts: {}
  };

  for (const [group, ids] of Object.entries(directoryGroups)) {
    fileStats.filesPerGroup[group] = ids.length;
  }

  fileNodes.forEach(node => {
    fileStats.nodeTypeCounts[node.type] = (fileStats.nodeTypeCounts[node.type] || 0) + 1;
  });

  // === Output ===
  const output = {
    scriptCompleted: true,
    directoryGroups,
    nodeTypeGroups,
    crossCategoryEdges,
    interGroupImports,
    intraGroupDensity,
    patternMatches,
    deploymentTopology,
    dataPipeline,
    docCoverage,
    dependencyDirection,
    fileStats,
    fileFanIn: fanIn,
    fileFanOut: fanOut
  };

  fs.writeFileSync(outputPath, JSON.stringify(output, null, 2), 'utf-8');
  console.log(`Analysis complete. ${fileNodes.length} file nodes, ${importEdges.length} import edges.`);
  process.exit(0);

} catch (err) {
  console.error('Fatal error:', err.message);
  console.error(err.stack);
  process.exit(1);
}

import re
import sys
from pathlib import Path

type_getter = {
    'int': 'getValueAsInt()',
    'size_t': 'getValueAsLong()',
    'long': 'getValueAsLong()',
    'std::string': 'getValueAsString()',
    'bool': 'getValueAsBool()',
    'double': 'getValueAsDouble()',
    'float': 'getValueAsFloat()'
}
type_setter = {
    'int': 'setValueFromInt({})',
    'size_t': 'setValueFromLong({})',
    'long': 'setValueFromLong({})',
    'std::string': 'setValueFromString({})',
    'bool': 'setValueFromBool({})',
    'double': 'setValueFromDouble({})',
    'float': 'setValueFromFloat({})'
}

def parse_oso_file(filepath):
    with open(filepath, encoding='utf-8') as f:
        content = f.read()

    includes = re.findall(r'^\s*#include\s+[<"].+[>"]', content, re.MULTILINE)

    # Recursively parse namespaces and classes, build a tree
    def parse_ns_block(block):
        ns_match = re.match(r'namespace\s+(\w+)\s*\{(.*)\}\s*$', block, re.DOTALL)
        if not ns_match:
            return None
        ns = ns_match.group(1)
        body = ns_match.group(2)

        # Find all sub-namespaces
        sub_ns_pattern = re.compile(r'(namespace\s+\w+\s*\{(?:[^{}]|\{[^{}]*\})*\})', re.DOTALL)
        sub_ns_blocks = sub_ns_pattern.findall(body)
        sub_ns = [parse_ns_block(sub_block) for sub_block in sub_ns_blocks]

        # Remove sub-namespace content, the rest are class definitions
        body_wo_subns = body
        for sub_block in sub_ns_blocks:
            body_wo_subns = body_wo_subns.replace(sub_block, '')

        # Find all classes
        class_pattern = re.compile(r'class\s+(\w+)\s*\{([^}]*)\};', re.DOTALL)
        classes = []
        for class_match in class_pattern.finditer(body_wo_subns):
            class_name = class_match.group(1)
            class_body = class_match.group(2)
            field_lines = []
            for line in class_body.splitlines():
                line = line.strip()
                if not line or line.startswith('//') or line == 'public:':
                    continue
                field_lines.append(line)

            fields = []
            names = set()
            for idx, line in enumerate(field_lines, 1):
                m = re.match(r'^(\w+)\s*:\s*([\w:]+)\s*;\s*$', line)
                m_def = re.match(r'^(\w+)\s*:\s*([\w:]+)\s*:\s*default\s*=\s*(.+);$', line)
                if m_def:
                    name, typ, default = m_def.groups()
                    has_default = True
                elif m:
                    name, typ = m.groups()
                    default = None
                    has_default = False
                else:
                    print(f"Syntax error: line {idx} `{line}` does not match `name : type ;` or `name : type : default = value;` format")
                    sys.exit(1)
                if name in names:
                    print(f"Syntax error: duplicate variable name `{name}` (line {idx})")
                    sys.exit(1)
                if typ not in type_getter:
                    print(f"Syntax error: type `{typ}` is not supported (line {idx})")
                    sys.exit(1)
                names.add(name)
                if has_default:
                    default = default.strip()
                    if typ == 'std::string' and not default.startswith('"'):
                        default = f'"{default}"'
                    if typ == 'bool':
                        default = 'true' if default.lower() == 'true' else 'false'
                fields.append({'name': name, 'type': typ, 'default': default, 'has_default': has_default})

            classes.append({'class_name': class_name, 'fields': fields})

        return {'namespace': ns, 'sub_ns': [s for s in sub_ns if s], 'classes': classes}

    # Find all top-level namespaces
    pattern = re.compile(r'(namespace\s+\w+\s*\{(?:[^{}]|\{[^{}]*\})*\})', re.DOTALL)
    ns_blocks = pattern.findall(content)
    ns_tree = [parse_ns_block(block) for block in ns_blocks if parse_ns_block(block)]

    # Handle global classes
    content_wo_ns = content
    for block in ns_blocks:
        content_wo_ns = content_wo_ns.replace(block, '')
    class_pattern = re.compile(r'class\s+(\w+)\s*\{([^}]*)\};', re.DOTALL)
    global_classes = []
    for class_match in class_pattern.finditer(content_wo_ns):
        class_name = class_match.group(1)
        class_body = class_match.group(2)
        field_lines = []
        for line in class_body.splitlines():
            line = line.strip()
            if not line or line.startswith('//') or line == 'public:':
                continue
            field_lines.append(line)

        fields = []
        names = set()
        for idx, line in enumerate(field_lines, 1):
            m = re.match(r'^(\w+)\s*:\s*([\w:]+)\s*;\s*$', line)
            m_def = re.match(r'^(\w+)\s*:\s*([\w:]+)\s*:\s*default\s*=\s*(.+);$', line)
            if m_def:
                name, typ, default = m_def.groups()
                has_default = True
            elif m:
                name, typ = m.groups()
                default = None
                has_default = False
            else:
                print(f"Syntax error: line {idx} `{line}` does not match `name : type ;` or `name : type : default = value;` format")
                sys.exit(1)
            if name in names:
                print(f"Syntax error: duplicate variable name `{name}` (line {idx})")
                sys.exit(1)
            if typ not in type_getter:
                print(f"Syntax error: type `{typ}` is not supported (line {idx})")
                sys.exit(1)
            names.add(name)
            if has_default:
                default = default.strip()
                if typ == 'std::string' and not default.startswith('"'):
                    default = f'"{default}"'
                if typ == 'bool':
                    default = 'true' if default.lower() == 'true' else 'false'
            fields.append({'name': name, 'type': typ, 'default': default, 'has_default': has_default})

        global_classes.append({'class_name': class_name, 'fields': fields})

    return includes, ns_tree, global_classes

def wrap_namespaces(code: str, ns_list):
    """Recursively wrap code with namespaces, ensuring correct indentation and nesting"""
    if not ns_list:
        return code
    # Reverse namespace list, outermost first
    ns_list = ns_list[::-1]
    for ns in ns_list:
        code = f'namespace {ns} {{\n' + '\n'.join('    ' + line if line.strip() else '' for line in code.splitlines()) + '\n}'
    return code

def generate_hpp(info):
    # Member variable definitions
    members = []
    for f in info['fields']:
        if f['has_default']:
            members.append(f'    {f["type"]} {f["name"]}{{ {f["default"]} }};')
        else:
            members.append(f'    {f["type"]} {f["name"]}{{}};')
    members = '\n'.join(members)

    # Constructor deserialization (fixed indentation)
    from_assembly = []
    for f in info['fields']:
        from_assembly.append(
            f'    auto {f["name"]}Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable${f["name"]}$"));\n'
            f'    if (!{f["name"]}Item) {{\n'
            f'        throw std::runtime_error("$variable${f["name"]} is not found");\n'
            f'    }}\n'
            f'    {f["name"]} = {f["name"]}Item->{type_getter.get(f["type"], "getValueAsString()")};'
        )
    from_assembly_code = '\n'.join(from_assembly)

    # to assembly
    to_assembly = []
    for f in info['fields']:
        setter = type_setter.get(f["type"], 'setValueFromString({})').format(f["name"])
        to_assembly.append(
            f'    auto {f["name"]}Item = std::make_shared<rw::oso::ObjectStoreItem>();\n'
            f'    {f["name"]}Item->setName("$variable${f["name"]}$");\n'
            f'    {f["name"]}Item->{setter};\n'
            f'    assembly.addItem({f["name"]}Item);'
        )
    to_assembly_code = '\n'.join(to_assembly)

    copy_ctor = '\n'.join([f'    {f["name"]} = obj.{f["name"]};' for f in info['fields']])
    assign = '\n'.join([f'        {f["name"]} = obj.{f["name"]};' for f in info['fields']])
    eq = ' && '.join([f'{f["name"]} == obj.{f["name"]}' for f in info['fields']])

    class_code = f'''
class {info["class_name"]}
{{
public:
    {info["class_name"]}() = default;
    ~{info["class_name"]}() = default;

    {info["class_name"]}(const rw::oso::ObjectStoreAssembly& assembly);
    {info["class_name"]}(const {info["class_name"]}& obj);

    {info["class_name"]}& operator=(const {info["class_name"]}& obj);
    operator rw::oso::ObjectStoreAssembly() const;
    bool operator==(const {info["class_name"]}& obj) const;
    bool operator!=(const {info["class_name"]}& obj) const;

public:
{members}
}};

inline {info["class_name"]}::{info["class_name"]}(const rw::oso::ObjectStoreAssembly& assembly)
{{
    auto isAccountAssembly = assembly.getName();
    if (isAccountAssembly != "$class${info["class_name"]}$")
    {{
        throw std::runtime_error("Assembly is not $class${info["class_name"]}$");
    }}
{from_assembly_code}
}}

inline {info["class_name"]}::{info["class_name"]}(const {info["class_name"]}& obj)
{{
{copy_ctor}
}}

inline {info["class_name"]}& {info["class_name"]}::operator=(const {info["class_name"]}& obj)
{{
    if (this != &obj) {{
{assign}
    }}
    return *this;
}}

inline {info["class_name"]}::operator rw::oso::ObjectStoreAssembly() const
{{
    rw::oso::ObjectStoreAssembly assembly;
    assembly.setName("$class${info["class_name"]}$");
{to_assembly_code}
    return assembly;
}}

inline bool {info["class_name"]}::operator==(const {info["class_name"]}& obj) const
{{
    return {eq};
}}

inline bool {info["class_name"]}::operator!=(const {info["class_name"]}& obj) const
{{
    return !(*this == obj);
}}
'''

    # Use recursive function to wrap namespaces
    ns_list = info.get('ns_list', [])
    class_code = wrap_namespaces(class_code.strip(), ns_list)

    return class_code + '\n'

def generate_ns_code(ns_node):
    code = ""
    # Generate all classes at this level
    for cls in ns_node['classes']:
        code += generate_hpp({'class_name': cls['class_name'], 'fields': cls['fields'], 'ns_list': []}) + "\n"
    # Recursively generate sub-namespaces
    for sub in ns_node['sub_ns']:
        code += generate_ns_code(sub)
    # If this level has content, wrap with this namespace
    if code.strip():
        code = f"namespace {ns_node['namespace']} {{\n" + '\n'.join('    ' + line if line.strip() else '' for line in code.splitlines()) + "\n}\n"
    return code

if __name__ == '__main__':
    oso_file = sys.argv[1] if len(sys.argv) > 1 else 'testClass.oso'
    output_dir = sys.argv[2] if len(sys.argv) > 2 else None

    includes, ns_tree, global_classes = parse_oso_file(oso_file)
    include_set = set(includes)
    includes = '\n'.join(sorted(include_set))

    all_code = []
    # Generate global classes first
    for cls in global_classes:
        all_code.append(generate_hpp({'class_name': cls['class_name'], 'fields': cls['fields'], 'ns_list': []}))
    # Then recursively generate namespace tree
    for ns in ns_tree:
        all_code.append(generate_ns_code(ns))

    hpp_code = f'''#pragma once

#include"oso_core.h"
{includes}

{''.join(all_code)}
'''
    filename = f"{Path(oso_file).stem}.hpp"
    if output_dir:
        hpp_path = Path(output_dir) / filename
    else:
        hpp_path = Path(oso_file).with_name(filename)
    hpp_path.parent.mkdir(parents=True, exist_ok=True)
    with open(hpp_path, 'w', encoding='utf-8') as f:
        f.write(hpp_code)
    print(f'Generated: {hpp_path}')
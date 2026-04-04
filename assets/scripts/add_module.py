import xml.etree.ElementTree as ET
import sys
import os

def add_module_to_keil(uvprojx_path, c_file_rel_path, inc_dir_rel_path):
    if not os.path.exists(uvprojx_path):
        print(f"Error: {uvprojx_path} does not exist.")
        sys.exit(1)

    # 1. Update compile_flags.txt
    flags_path = os.path.join(os.path.dirname(os.path.dirname(uvprojx_path)), "compile_flags.txt")
    if os.path.exists(flags_path):
        with open(flags_path, "r", encoding="utf-8") as f:
            flags = f.read()
        if f"-I{inc_dir_rel_path}" not in flags:
            with open(flags_path, "a", encoding="utf-8") as f:
                f.write(f"\n-I{inc_dir_rel_path}")
            print(f"Added {inc_dir_rel_path} to compile_flags.txt")

    # 2. Update .uvprojx
    # Keil XML can be finicky. ET preserves most of it.
    try:
        tree = ET.parse(uvprojx_path)
        root = tree.getroot()

        # Update IncludePath
        for cads in root.iter('Cads'):
            various = cads.find('VariousControls')
            if various is not None:
                inc_path_elem = various.find('IncludePath')
                if inc_path_elem is not None:
                    paths = inc_path_elem.text.split(';') if inc_path_elem.text else []
                    if inc_dir_rel_path not in paths:
                        paths.append(inc_dir_rel_path)
                        inc_path_elem.text = ';'.join(paths)
                        print(f"Added {inc_dir_rel_path} to .uvprojx IncludePath")

        # Update Groups
        groups_node = root.find('.//Groups')
        if groups_node is not None:
            app_group = None
            for g in groups_node.findall('Group'):
                gname = g.find('GroupName')
                if gname is not None and gname.text == 'APP':
                    app_group = g
                    break
            
            if app_group is None:
                app_group = ET.SubElement(groups_node, 'Group')
                gname = ET.SubElement(app_group, 'GroupName')
                gname.text = 'APP'
                print("Created APP group in .uvprojx")
            
            files_node = app_group.find('Files')
            if files_node is None:
                files_node = ET.SubElement(app_group, 'Files')
            
            # Check if file exists
            exists = False
            for f in files_node.findall('File'):
                fname = f.find('FileName')
                if fname is not None and fname.text == os.path.basename(c_file_rel_path):
                    exists = True
                    break
            
            if not exists:
                file_node = ET.SubElement(files_node, 'File')
                fn = ET.SubElement(file_node, 'FileName')
                fn.text = os.path.basename(c_file_rel_path)
                ft = ET.SubElement(file_node, 'FileType')
                ft.text = '1' # 1 means C file
                fp = ET.SubElement(file_node, 'FilePath')
                fp.text = c_file_rel_path
                print(f"Added {c_file_rel_path} to APP group in .uvprojx")

        tree.write(uvprojx_path, encoding='utf-8', xml_declaration=True)
        print("Keil project updated successfully.")

    except Exception as e:
        print(f"Error parsing .uvprojx: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python add_module.py <path_to.uvprojx> <relative_c_path> <relative_inc_dir>")
        sys.exit(1)
    add_module_to_keil(sys.argv[1], sys.argv[2], sys.argv[3])

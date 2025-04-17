import xml.etree.ElementTree as ET

def convert_object_to_tile(obj):
    x = obj.get("x")
    y = obj.get("y")
    width = obj.get("width")
    height = obj.get("height")
    
    return f'''        <tile>
            <position x="{x}" y="{y}" />
            <size w="{width}" h="{height}" />
        </tile>'''

def process_xml(input_file, output_file):
    tree = ET.parse(input_file)
    root = tree.getroot()
    
    tiles = [convert_object_to_tile(obj) for obj in root.findall(".//object")]
    
    with open(output_file, "w", encoding="utf-8") as f:
        f.write("    <floor>\n")
        f.write("\n".join(tiles))
        f.write("\n    </floor>\n")

if __name__ == "__main__":
    input_filename = "input.xml"
    output_filename = "output.xml"
    process_xml(input_filename, output_filename)
    print(f"Conversion complete. Output saved to {output_filename}")

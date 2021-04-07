import sys, re

def preprocess(source):
    source = re.sub(r"(?<=\/\*).*?(?=\*\/)", "", source)
    source = re.sub(r"(?<=\/\/).*?(?=\n)", "", source)

    source = source.replace("//", "")
    source = source.replace("/*", "")
    source = source.replace("*/", "")

    includes = re.findall(r'(?<=#include ).*?(?=\n)', source)

    for include in includes:
        path = include[1:-1]
        if include[0] == '<':
            # System header
            header_file = open(f'/usr/include/{path}')
            source = source.replace(f'#include {include}', header_file.read())
            header_file.close()

        elif include[0] == '"':
            # Working dir header
            header_file = open(path)
            source = source.replace(f'#include {include}', header_file.read())
            header_file.close()

    return source

def main(argv):
    input_file = open(argv[0], "r")
    source = input_file.read()
    input_file.close()

    pre = preprocess(source)

    output_file = open(argv[0][:-1] + "i", "w+")
    output_file.write(pre)
    output_file.close()
    
    

if __name__ == "__main__":
    main(sys.argv[1:])
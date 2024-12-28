
class graph_file:
    def __init__(self, name, file):
        self.name = name
        self.file = file
    def __str__(self):
        return f'{self.name}: {self.file}'
    def __repr__(self):
        return f'graph_file(\'{self.name}\', {self.file})'

test_list_result = []
test_files = []
test_index = 0

dirs_name=["BellmanFord", "ConnectedComponents", "MaxFlow", "DijkstraShortPath", "EulerianLoop",
           "EulerianPath", "HamiltonianLoop", "HamiltonianPath", "IsomorphismCheck",
           "LoadTest", "MaxClique", "PrintAllPaths"]

for dir_name in dirs_name:
    list_file = open(dir_name + "/testList_full.txt", "r")
    for index, line in enumerate(list_file):
        graph_name = "graph_" + str(test_index)
        params = line.replace("\n", "").split(" ")
        input_file = open(dir_name + "/" + params[1], "r")
        input_data = input_file.read().replace("\\", "\\\\")
        res_file = open(dir_name + "/" + params[1] + ".res", "r")
        res_data = res_file.read().rstrip()
        params = list(map(lambda x: "\"" + x + "\"", params))
        params[1] = graph_name
        test_files.append(graph_file(graph_name, input_data))
        test_str = ' + delimiter + '.join(map(lambda x: x, params))
        test_name = str(index) + ". " + params[0].replace("\"", "") + " " + graph_name
        full_test_str = '{{name: \"{0}\", test: {1}, res: `{2}`}}'.format(test_name, test_str, res_data)
        test_list_result.append(full_test_str)
        test_index+=1
    list_file.close()

template_file = open("emscripted_test_template.html", "r")
template_data = template_file.read()
template_file.close()

output_file = open("emscripted_test_output.html", "w")
get_test_function_body = ";\n".join(map(lambda res: "var " + res.name + "=" + "`" + res.file + "`", test_files))
get_test_function_body = get_test_function_body + ";\n\n"
get_test_function_body = get_test_function_body + "return [\n    "
get_test_function_body = get_test_function_body + ",\n    ".join(test_list_result)
get_test_function_body = get_test_function_body + "\n];\n"
output_file.write(template_data.replace("PLACE_TESTS_HERE", get_test_function_body))
output_file.close()


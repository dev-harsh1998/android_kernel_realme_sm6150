#!sh/bin
import os
import sys

PATTERN1 = "#include"

def get_include_name(inc_str):
	if inc_str.startswith(PATTERN1):
		return inc_str.lstrip(PATTERN1).strip()

	return None


def parse_include_str(inc_str):
	if type(inc_str) != type(""):
		return (None, None)

	parse_str = get_include_name(inc_str)
	if parse_str is None:
		return (None, None)

	parsed = None
	if parse_str.startswith('<'):
		parsed = parse_str.lstrip('<').rstrip('>').strip()

	if parse_str.startswith('"'):
		parsed =  parse_str.strip('"').strip()

	if parsed is None:
		return (None, None)

	if parsed.endswith(".h"):
		return (True, parsed)
	elif parsed.endswith(".dtsi"):
		return (False, parsed)
	else:
		return (None,None)

def print_result(depth, str):
	print("\t"*depth + str + "\n"),

def recycle_includes(root_dir, file_path, depth):
	if not os.path.isfile(file_path):
		return

	lines = None
	try:
		i_file = open(file_path)
		lines = i_file.readlines()
	except:
		print("Failed to open file %s"%file_path)

	for line in lines:
		is_h, name = parse_include_str(line)
		if is_h is None:
			continue
		print_result(depth, name)
		if is_h is False:
			depth += 1
			new_file = os.path.join(root_dir, name)
			recycle_includes(root_dir, new_file, depth)
			depth -= 1


args = os.sys.argv
if len(args) < 2:
	print("Usages: python dts_parse.py file.dtsi")
	sys.exit()

file_path = args[1]
if os.path.isfile(file_path):
	root_path = os.path.dirname(file_path)
	file_name = file_path
else:
	root_path = os.path.curdir()
	file_name = os.path.join(root_path, file_path)

recycle_includes(root_path, file_name, 0)
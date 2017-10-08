
import argparse
import re
import errno
import os
import os.path as path
from shutil import copyfile



cpp_comment_pat = re.compile(r'^(\s*)//( (?=[^ ]))?(.*$)')

# only uncomments simple //
def cpp_uncomment_line(line) :
	m = cpp_comment_pat.match(line)
	if m :
		line = m.group(1) + m.group(3)
	return line

def parse(infile, outfile) :

	# Read state
	modified = False
	out_lines = []
	remove = False
	comment = False
	uncomment = False
	delete_file = False

	with open(infile, 'r') as f :
		for line in f :

			# DELETE tag
			if 'CGRA_DELETE_FILE' in line :
				delete_file = True
				modified = True
				break

			# REMOVE tags
			elif 'CGRA_REMOVE' in line :
				modified = True
				continue

			elif 'CGRA_BEGIN_REMOVE' in line :
				if comment :
					raise Exception('Can not use CGRA_BEGIN_REMOVE within a CGRA_BEGIN_COMMENT block')
				elif uncomment : 
					raise Exception('Can not use CGRA_BEGIN_REMOVE within a CGRA_BEGIN_UNCOMMENT block')
				remove = True
				modified = True
			elif 'CGRA_END_REMOVE' in line :
				if not remove : 
					raise Exception('Can not use CGRA_END_REMOVE without a CGRA_BEGIN_REMOVE tag')
				remove = False


			# COMMENT tags
			elif 'CGRA_BEGIN_COMMENT' in line :
				if remove :
					raise Exception('Can not use CGRA_BEGIN_COMMENT within a CGRA_BEGIN_REMOVE block')
				elif uncomment : 
					raise Exception('Can not use CGRA_BEGIN_COMMENT within a CGRA_BEGIN_UNCOMMENT block')
				comment = True
				modified = True
			elif 'CGRA_END_COMMENT' in line :
				if not comment : 
					raise Exception('Can not use CGRA_END_COMMENT without a CGRA_BEGIN_COMMENT tag')
				comment = False


			# UNCOMMENT tags
			elif 'CGRA_BEGIN_UNCOMMENT' in line :
				if remove :
					raise Exception('Can not use CGRA_BEGIN_UNCOMMENT within a CGRA_BEGIN_REMOVE block')
				elif comment : 
					raise Exception('Can not use CGRA_BEGIN_UNCOMMENT within a CGRA_BEGIN_COMMENT block')
				uncomment = True
				modified = True
			elif 'CGRA_END_UNCOMMENT' in line :
				if not uncomment : 
					raise Exception('Can not use CGRA_END_UNCOMMENT without a CGRA_BEGIN_UNCOMMENT tag')
				uncomment = False


			# Regular line
			else :
				if remove :
					pass
				elif comment :
					# TODO better commenting
					out_lines.append('//' + line)
				elif uncomment :
					out_lines.append(cpp_uncomment_line(line) + "\n")
				else :
					out_lines.append(line)
		# }
	# }

	if modified :
		if not delete_file :
			with open(outfile, 'w') as f :
				for line in out_lines :
					f.write(line)
				# }
			# }
		# }
		return True
	# }

	# No tags were found, simply copy the file
	copyfile(infile, outfile)
	return False
# }



# create directory (in thread safe way) if it doesn't exist
def create_directory(new_dir, i=0) :
	try:
		os.makedirs(mod_dir)
	except OSError as e:
		if e.errno != errno.EEXIST:
			raise
		else :
			pass # deal with dir exisiting?
	# }
# }



def process_file(input_file, output_dir) :
	if not path.isfile(output_dir) :
		raise Exception(output_dir + ' is not a valid file.')

	head, tail = path.split(file_dir)
	output_file = path.join(output_dir, tail)

	if parse(input_file, output_file) :
		print('(modified) ' + output_file)
# }



def process_directory(base_dir, output_dir) :

	file_count = 0
	modified_count = 0

	for root, dirs, files in os.walk(base_dir) :
		relative_path = path.relpath(root, base_dir)

		new_dir = path.join(output_dir, relative_path)
		create_directory(new_dir)

		for file in files :
			input_file = path.join(root, file)
			output_file = path.join(output_dir, relative_path, file)
			if parse(input_file, output_file) :
				modified_count += 1
				print('(modified) ' + output_file)
			# }
			file_count += 1
		# }
	# }

	print("Copied {0} total files".format(file_count))
	print("Modified {0} out of {1} files".format(modified_count, file_count))
# }




def main() :

	# parse arguments
	parser = argparse.ArgumentParser(description='Parses a project for CGRA meta tags and produces a seperate modified project based on the tag functions.')
	parser.add_argument(
		'in', metavar='In', type=str, nargs='+',
		help='input directories to parse'
	)
	parser.add_argument(
		'-o', metavar='Out', type=str, nargs=1, default=[os.getcwd()],
		help='output directory for all modified project files'
	)
	args = vars(parser.parse_args())



	# get and validate output directory
	output_dir = args['o'][0]
	if not path.exists(output_dir) :
		raise Exception(output_dir + ' does not exist as an output directory.')
	if not path.isdir(output_dir) :
		raise Exception(output_dir + ' is not a valid output directory.')

	# for input directories
	for in_arg in args['in'] :
		if not path.exists(in_arg) :
			raise Exception(in_arg + ' does not exist.')

		# for directories
		elif path.isdir(in_arg) :
			head, tail = path.split(in_arg)
			mod_output_dir = path.join(output_dir, tail)
			create_directory(mod_output_dir)
			process_directory(in_arg, mod_output_dir)

		# for files
		else :
			process_file(in_arg, output_dir)
		# }
	# }
# }



if __name__ == '__main__':
	main()
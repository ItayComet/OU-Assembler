/*The pre assembler is a stage where we take an '.as' file and create a new file with the '.am' extension.
The '.am' file is the same as the '.as' file, and the only differnce is that the macros, if there are any, have been spreaded out,there are no macro line in the '.am' files.
	@param fp: a pointer to the stream of the '.as' file
	@param name: the name of the file without the extension
*/
int pre_assemble(FILE* fp, char* name);

# lang-mu
Language for working with matrices and vectors

### Build
We are using Code::Blocks for build.  
First you will need to install flex and bison.  
Then in Code::Blocks  
``Settings > Compiler > Global compiler settings > Other settings > Advanced options``  
Make following changes:
*   Command: ``Compile single file to object file``  
	Source ext: ``l``  
	Command line ``macro: flex -o$file_dir/$file_name.scanner.c $file``  
	Generated files: ``$file_dir/$file_name.scanner.c``
*   Command: ``Compile single file to object file``  
	Source ext: ``y``  
	Command line ``macro: bison -v -d $file -o $file_dir/$file_name.parser.c``  
	Generated files:  
	``$file_dir/$file_name.parser.c``  
	``$file_dir/$file_name.parser.h``

Now everything should be ready for build.

# Sylvain Lefebvre 2010-04-01

import sys;
import re;
import datetime;
import os;
import string;
import base64;

#
#   TODO:  currently includes could go into an infinite loop ...!
#   TODO:  support for ifndef
#

try:
	__file__
except:
	sys.argv = [sys.argv[0], 'shader_tutorial0.vp']

if len(sys.argv) < 2:
	print('AutoBindShader - ERROR: not enough arguments: {0} <shader>'.format(sys.argv[0]));
	sys.exit()

print(sys.argv[1])
shader  = re.search( '(\w*)\.\w+$' , sys.argv[1] ).group(1)
path	= re.search( '(.*)\.\w+$' , sys.argv[1] ).group(1)
if sys.platform.startswith('win32'):
	path	= path.replace('/','\\')
	dir	 = path.rpartition('\\')[0] + path.rpartition('\\')[1]
else:
	dir	 = path.rpartition('/')[0] + path.rpartition('/')[1]

defstack = []; # stack for nested define conditions

now = datetime.datetime.now()
print(' [AutoBindShader] - v0.21 - ' + now.strftime("%Y-%m-%d %H:%M") + ' <' + shader + '>  ');

fcpp	= open(shader+".h","w")

allUniforms   = dict()  # GLSL / HLSL
# use a dictionnary to prevent uniform encapsulated in ifdef to
allDefaults   = {}  # Default settings
allTechniques = []  # HLSL
allTweaks	 = []  # Tweaks for AntTweakBar
allDefines	= {}  # Defines
allDefineTweaks	= {}  # Defines
definesOpt	= {}  # Defines options
allStringInc  = {}  # String includes

# useful methods

### Encode a string
def encode(str) :
	return base64.standard_b64encode(str.encode('ascii', 'ignore')).decode('ascii')

### Parse all variables
def parseVars( fname, notweak ) :
	print(' [AutoBindShader] parsing   \'{0}\''.format(fname))
	fshader = open(fname,"r")
	lines   = fshader.readlines()
	for l in lines:
		# print("#{0}#".format(l))
		if l.find("#include") > -1:
			include = dir + re.match('#include\s+\"(.*)\"',l).group(1)
			matched_notweak	= re.match('.*//\s*notweak\s*',l);
			parseVars( include, (matched_notweak != None) | notweak )
		if l.find("#string") > -1:
			string = re.match('#string\s+(.*)',l).group(1)
			allStringInc[string] = 1
		if re.match(".*\s*uniform\s+",l) != None:
					var	 = re.match('.*uniform\s+(?:layout\(.*\)\s+)?(\w+)\s+\*?(\w+)\s*(?:\[\s*(.*)\s*\])?\s*(?:=\s*(.*)\s*)?;',l)
					# print(var.group(0))
					# print(var.group(1))
					# print(var.group(2))
					allUniforms[var.group(2)] = var # only store once uniforms to handle ifdef 
					#TODO if already present check that type are compatible (group(1))
					if var.group(3) != None:
							print(' [AutoBindShader] \'{0}\' is an array of size \'{1}\''.format(var.group(2),var.group(3)) )
					if var.group(4) != None:
						allDefaults[ var.group(2) ] = var.group(4);
						print(' [AutoBindShader] \'{0}\' defaults to \'{1}\''.format(var.group(2),var.group(4)) )
					if  not notweak:
						matched = re.match('.*;\s*//\s*tweak\s+(\w+)\s*(.*)$',l)
						if matched != None:
							print(' [AutoBindShader] tweak for \'{0}\', type is \'{1}\''.format(var.group(2),matched.group(1)) )
							allTweaks.append( [var,matched] )
		if l.find("technique") > -1:
			var = re.match('technique\s+(\w+)',l)
			allTechniques.append( var )
	return

### Append a file into a string
def appendFileToString( fname, notweak ) :
		print(' [AutoBindShader] including \'{0}\''.format( fname ))
		file  = open(fname,"r")
		lines = file.readlines()
		count = 0
		str   = ''
		for l in lines:
				count = count + 1;
				if count > 128:
					# interrupt and restart string
					fcpp.write( encode(str) + '") + decode("');
					str = '';
					count = 0;
				matched_ignore	= re.match('.*//\s*ignore\s*',l);
				if matched_ignore != None:
					str = str + l;
					continue
				# check line content
				matched_if	= re.match('^#ifdef\s+(\w*)\s*(?://(.*)\s*)?$',l);
				matched_ifnot = re.match('^#ifndef\s+(\w*)\s*(?://(.*)\s*)?$',l);
				if matched_if != None:
					# interrupt string
					fcpp.write( encode(str) + '") + \n' );
					str = '';
					# add condition
					allDefines[ matched_if.group(1) ] = 1
					if not notweak:
						allDefineTweaks[ matched_if.group(1) ] = 1
					fcpp.write( '( ({0}) ? (\n'.format(matched_if.group(1)) )
					# options
					if matched_if.group(2) != None:
						definesOpt[ matched_if.group(1) ] = matched_if.group(2)
					# restart string
					fcpp.write(' decode("')
					# push condition
					defstack.append( matched_if.group(1) )
				elif matched_ifnot != None:
					# interrupt string
					fcpp.write( encode(str) + '") + \n' )
					str = '';
					# add condition
					allDefines[ matched_ifnot.group(1) ] = 1
					if not notweak:
						allDefineTweaks[ matched_ifnot.group(1) ] = 1
					fcpp.write( '( ( ! {0}) ? (\n'.format(matched_ifnot.group(1)) )
					# options
					if matched_ifnot.group(2) != None:
						definesOpt[ matched_ifnot.group(1) ] = matched_ifnot.group(2)
					# restart string
					fcpp.write(' decode("')
					# push condition
					defstack.append( matched_ifnot.group(1) )
				elif l.find("#else") > -1:
					# interrupt string
					fcpp.write( encode(str) + '")\n' )
					str = '';
					# add else
					fcpp.write( ') : (\n' )
					# restart string
					fcpp.write(' decode("')
					# remember a 'else' was encountered
					allDefines[ defstack[len(defstack)-1] ] = 0
				elif l.find("#endif") > -1:
					# interrupt string
					fcpp.write( encode(str) + '")\n' )
					str = '';
					# end condition, depends on whether the else was encountered
					if allDefines[ defstack[len(defstack)-1] ] == 1:
						fcpp.write( ') : std::string("") ) + \n' )
					else:
						fcpp.write( ') ) + \n' )
					# restart string
					fcpp.write(' decode("')
					# pop last condition
					defstack.pop()
				elif l.find("#include") > -1:
					include = dir + re.search('#include\s+\"(.*)\"',l).group(1)
					matched_notweak	= re.match('.*//\s*notweak\s*',l);
					# interrupt string
					fcpp.write( encode(str + '\n') + '") + decode("' )
					str = '';
					# include
					appendFileToString( include, (matched_notweak != None) | notweak )
					# resume
					fcpp.write('") + decode("' )
				elif l.find("#string") > -1:
					string = re.search('#string\s+(.*)',l).group(1)
					fcpp.write( encode(str) + '") + {0} + decode("'.format(string) )
					str = ''
				else:
					str = str + l;
		fcpp.write( encode(str) )
		return

### Import a file into a string, adding the string definition to the cpp code
def importFile( fname ) :
	fcpp.write('decode("')
	appendFileToString( fname, False )
	fcpp.write('");\n')
	return

### Converts a string of a shader type into a cpp type
def shaderType2CppType( stype ) :
	if stype == "vec2":
		return "v2f"
	if stype == "vec3":
		return "v3f"
	if stype == "vec4":
		return "v4f"
	return stype

### Check whether this is a compute shader
isComputeShader = False
if os.path.isfile(path+".cs"):
	isComputeShader = True
className = 'Shader'
if isComputeShader:
	className = 'Compute'
fcpp.write('// Automatically generated by AutoBindShader - ' + now.strftime("%Y-%m-%d %H:%M") + '\n')
fcpp.write('#pragma once\n\n')
fcpp.write('#include <LibSL/GLHelpers/AutoBindShader.h>\n')
fcpp.write('#define vec2(x,y)	 V2F(x,y)\n')
fcpp.write('#define vec3(x,y,z)   V3F(x,y,z)\n')
fcpp.write('#define vec4(x,y,z,w) V4F(x,y,z,w)\n')
# callback forward def
fcpp.write('namespace AutoBindShader {\n')
fcpp.write('class {0}_precompiled\n'.format(shader))
fcpp.write('{\n');
# public members
fcpp.write('protected:\n');
fcpp.write('    GLuint   m_TypeIn;\n');
fcpp.write('    GLuint   m_TypeOut;\n');
fcpp.write('    GLuint   m_MaxVerticesOut;\n');
# public members
fcpp.write('public:\n');
# struct for callbacks on defines
fcpp.write('#ifdef TW_INCLUDED\n')
fcpp.write('typedef struct {{ bool *member; {1}<{0}_precompiled> *owner; }} t_cb_data;\n'.format(shader,className));
fcpp.write('#endif //TW_INCLUDED\n')

# init

fcpp.write('   std::string name() {{ return "{0}"; }}\n'.format(shader))

fcpp.write('		std::string csCode() {')
fcpp.write('		  std::string cs_code;')
if os.path.isfile(path+".cs"):
	fcpp.write('		  cs_code = ')
	importFile( path+".cs" );
	parseVars( path+".cs", False )
fcpp.write('	return cs_code; }\n')

fcpp.write('		std::string vpCode() {')
fcpp.write('		  std::string vp_code;')
if os.path.isfile(path+".vp"):
	if isComputeShader:
		print(' [AutoBindShader] FATAL ERROR, cannot mix compute shader (cs) and rendering pipeline shaders (vp)')
		sys.exit(-1)
	fcpp.write('		  vp_code = ')
	importFile( path+".vp" )
	parseVars( path+".vp", False )
fcpp.write('	return vp_code; }\n')

fcpp.write('		std::string fpCode() {')
fcpp.write('		  std::string fp_code;')
if os.path.isfile(path+".fp"):
	if isComputeShader:
		print(' [AutoBindShader] FATAL ERROR, cannot mix compute shader (cs) and rendering pipeline shaders (fp)')
		sys.exit(-1)
	fcpp.write('		  fp_code = \n')
	importFile( path+".fp" )
	parseVars( path+".fp", False )
fcpp.write('	return fp_code; }\n')

fcpp.write('		LibSL::GLHelpers::GLShader::t_GeometryShaderNfo gsCode() {')
fcpp.write('		  LibSL::GLHelpers::GLShader::t_GeometryShaderNfo gs;')
if os.path.isfile(path+".gs"):
	if isComputeShader:
		print(' [AutoBindShader] FATAL ERROR, cannot mix compute shader (cs) and rendering pipeline shaders (gs)')
		sys.exit(-1)
	fcpp.write('		  std::string gs_code = \n')
	importFile( path+".gs" )
	parseVars( path+".gs", False )
	fcpp.write('		  gs.code		    = gs_code.c_str();\n')
	fcpp.write('		  gs.typeIn		    = m_TypeIn; \n')
	fcpp.write('		  gs.typeOut		= m_TypeOut;\n')
	fcpp.write('		  gs.maxVerticesOut = m_MaxVerticesOut;\n')
fcpp.write('	return gs; }\n')

# init parameters
fcpp.write('void initParameters(LibSL::GLHelpers::GL{0}& shader,bool firstInit) {{\n'.format(className));
for key, val in allUniforms.items():
	fcpp.write('		  {0}.init( shader , "{1}" );\n'.format(val.group(2),val.group(2)))
# inital values
fcpp.write('		  if ( firstInit ) {\n');
fcpp.write('			shader.begin();\n');
for key, val in allUniforms.items():
	if val.group(4) != None:
		fcpp.write('			{0}.set( ({1}){2} );\n'.format(val.group(2),shaderType2CppType(val.group(1)),val.group(4)))
fcpp.write('			shader.end();\n');
fcpp.write('		  }\n');
fcpp.write('}\n');

# constructor
fcpp.write('		{0}_precompiled() {{\n'.format(shader))
# -> init all defines to 'false'
for v in allDefines.keys():
	fcpp.write('			' + v + ' = false;\n' )
fcpp.write('			}\n')
# access to uniforms
for key, val in allUniforms.items(): # if uniform in ifdef the commented 'type' might be wrong
	fcpp.write('		LibSL::GLHelpers::GLParameter ' + val.group(2) + '; /// ' + val.group(1) + '\n' )
# access to defines
for v in allDefines.keys():
	fcpp.write('		bool		' + v + ';\n' )
# strings for code customization
for s in allStringInc.keys():
	fcpp.write('		std::string ' + s + ';\n')
# other accessors
#if isComputeShader:
#	fcpp.write('		void	  run(const LibSL::Math::v3i& numGroups)	  { m_Shader.run(numGroups);   }\n')
# tweaking with AntTweakBar
fcpp.write('#ifdef TW_INCLUDED\n')
fcpp.write('public:\n')
for v in allTweaks:
	fcpp.write('		{0} m_tweak_{1};\n'.format(shaderType2CppType(v[0].group(1)),v[0].group(2)) )
fcpp.write('public:\n')
fcpp.write('		TwBar *makeTwBar({1}<{0}_precompiled> *owner)\n'.format(shader,className))
fcpp.write('		{\n');
fcpp.write('			TwBar *bar = TwNewBar("bar_{0}");\n'.format(shader));
# -> tweaks
for v in allTweaks:
	if v[0].group(1) == "float":
		fcpp.write('			TwAddVarRW(bar, "{0}", TW_TYPE_FLOAT, &m_tweak_{0} ,"label=\'{0}\' precision=5 {1}");\n'.format(v[0].group(2),v[1].group(2).rstrip('\n\r ')) )
	if v[0].group(1) == "int":
		if v[1].group(1) == "enum":
				enum = v[1].group(2).split(',')
				fcpp.write('			TwEnumVal enum_{0}[] = {{'.format(v[0].group(2)))
				n = 0
				for e in enum:
						fcpp.write('{{ {0},\"{1}\" }}'.format(n,e))
						n = n + 1
						if n < len(enum):
								fcpp.write(',');
				fcpp.write('};\n')
				fcpp.write('			TwType type_{0} = TwDefineEnum("{0}", enum_{0}, {1});\n'.format(v[0].group(2).rstrip('\n\r '),len(enum)));
				fcpp.write('			TwAddVarRW(bar, "{0}", type_{0}, &m_tweak_{0} ,"label=\'{0}\'");\n'.format(v[0].group(2)) )
		else:
				fcpp.write('			TwAddVarRW(bar, "{0}", TW_TYPE_INT32, &m_tweak_{0} ,"label=\'{0}\' {1}");\n'.format(v[0].group(2),v[1].group(2).rstrip('\n\r ')) )
	if v[0].group(1) == "vec3":
		if v[1].group(1) == "direction":
			fcpp.write('			TwAddVarRW(bar, "{0}", TW_TYPE_DIR3F,   &m_tweak_{0}[0] ,"label=\'{0}\' {1}");\n'.format(v[0].group(2),v[1].group(2).rstrip('\n\r ')) )
		elif v[1].group(1) == "color":
			fcpp.write('			TwAddVarRW(bar, "{0}", TW_TYPE_COLOR3F, &m_tweak_{0}[0] ,"label=\'{0}\' {1}");\n'.format(v[0].group(2),v[1].group(2).rstrip('\n\r ')) )
# -> defines as switches
if len(allDefineTweaks.keys()) > 0:
	fcpp.write('			t_cb_data *cbd;\n');
	for v in allDefineTweaks.keys():
		fcpp.write('			cbd = new t_cb_data; cbd->member = &{0}; cbd->owner = owner;\n'.format(v));
		opt = '';
		if v in definesOpt.keys():
			opt = definesOpt[v]
		fcpp.write('			TwAddVarCB(bar, "{1}", TW_TYPE_BOOLCPP, defineSetCallBack, defineGetCallBack, cbd ,"label=\'{1}\' {2}");\n'.format(shader,v,opt.rstrip('\n\r ')) )
fcpp.write('			return bar;\n');
fcpp.write('		}\n');
# init tweaks with default values
fcpp.write('		void initTweaks()\n');
fcpp.write('		{\n');
for v in allTweaks:
	name = v[0].group(2);
	if name in allDefaults:
		fcpp.write('			m_tweak_{0} = {1};\n'.format( name,allDefaults[name] ) )
	else:
		fcpp.write('			m_tweak_{0} = 0;\n'.format( name ) )
fcpp.write('		}\n');
fcpp.write('		void commitTweaks()\n');
fcpp.write('		{\n');
for v in allTweaks:
	fcpp.write('				{0}.set( m_tweak_{0} );\n'.format( v[0].group(2)) )
fcpp.write('		}\n');
# saving tweaks
fcpp.write('		void saveTweaks(const char *fname)\n')
fcpp.write('		{\n')
fcpp.write('		   FILE *f = NULL;\n')
fcpp.write('		   fopen_s(&f,fname,"wb");\n')
fcpp.write('		   sl_assert(f != NULL);\n')
for v in allTweaks:
	fcpp.write('		   fwrite(&m_tweak_{0},1,sizeof(m_tweak_{0}),f);\n'.format( v[0].group(2)) )
fcpp.write('		   fclose(f);\n');
fcpp.write('		}\n');
# loading tweaks
fcpp.write('		void loadTweaks(const char *fname)\n')
fcpp.write('		{\n')
fcpp.write('		   FILE *f = NULL;\n')
fcpp.write('		   fopen_s(&f,fname,"rb");\n')
fcpp.write('		   sl_assert(f != NULL);\n')
for v in allTweaks:
	fcpp.write('		   fread(&m_tweak_{0},1,sizeof(m_tweak_{0}),f);\n'.format( v[0].group(2)) )
fcpp.write('		   fclose(f);\n');
fcpp.write('		}\n');

# callbacks for defines
fcpp.write('		static void TW_CALL defineGetCallBack(void *value	  , void *ptr) {\n');
fcpp.write('		  t_cb_data* d = (t_cb_data*) ptr; *(bool*)value = *(d->member); }}\n'.format(shader) )
fcpp.write('			static void TW_CALL defineSetCallBack(const void *value, void *ptr) {\n');
fcpp.write('		  t_cb_data* d = (t_cb_data*) ptr; *(d->member) = *(const bool*)value; d->owner->init(); }}\n'.format(shader) )
# in case AntTweakBar is not here
fcpp.write('#else  //TW_INCLUDED\n')
fcpp.write('    	void initTweaks()   { }\n');
fcpp.write('		void commitTweaks() { }\n');
fcpp.write('#endif //TW_INCLUDED\n')
# done
fcpp.write('}; // class\n')

# create shader class
fcpp.write('typedef AutoBindShader::{1}<{0}_precompiled> {0};'.format(shader,className))

fcpp.write('} // namespace AutoBindShader;\n')
fcpp.write('#undef vec2\n')
fcpp.write('#undef vec3\n')
fcpp.write('#undef vec4\n')

fcpp.close();

print(' [AutoBindShader] success, generated <{0}>'.format(shader+".h"))

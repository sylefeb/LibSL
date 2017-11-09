/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@sophia.inria.fr

                  Simple Library for Graphics (LibSL)

This software is a computer program whose purpose is to offer a set of
tools to simplify programming real-time computer graphics applications
under OpenGL and DirectX.

This software is governed by the CeCILL-C license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-C
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-C license and that you accept its terms.
-------------------------------------------------------------------- */
#include "precompiled.h"

#include <LibSL/System/System.h>
using namespace LibSL::System;
#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;

// -----------

#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

// -----------

void test_system()
{
  cerr << endl;
  cerr << "---------------------------" << endl;
  cerr << " LibSL::System::Time " << endl;
  cerr << "---------------------------" << endl;

  cerr << " measuring sleep(1 sec) duration ... ";
  t_time tm_start = System::Time::milliseconds();
#ifdef WIN32
  Sleep(1000);
#else  
  sleep(1);
#endif
  t_time tm_stop = System::Time::milliseconds();
  cerr << "done" << endl;
  cerr << sprint(" sleep(1 sec) duration is measured to be %d ms\n",tm_stop-tm_start);

  cerr << endl;
  cerr << "---------------------------" << endl;
  cerr << " LibSL::CppHelpers binary streams " << endl;
  cerr << "---------------------------" << endl;

  {
    float f0=1.2345f;
    obinstream fout("test.tmp",ios::binary);
    fout << f0;
    fout.close();

    float f1=0.0f;
    ibinstream fin("test.tmp",ios::binary);
    fin >> f1;
    fin.close();

    cerr << sprint("Wrote %f, read %f\n",f0,f1);
  }

  {
    string s0="abcdefg";
    obinstream fout("test.tmp",ios::binary);
    fout << s0;
    fout.close();

    string s1;
    ibinstream fin("test.tmp",ios::binary);
    fin >> s1;
    fin.close();

    cerr << sprint("Wrote '%s', read '%s'\n",s0.c_str(),s1.c_str());
  }

  cerr << endl;
  cerr << "---------------------------" << endl;
  cerr << " LibSL::CppHelpers::BasicParser " << endl;
  cerr << "---------------------------" << endl;

  {
    const char *str = " 5,  ab cdefg ;\"blabla blabla\" \n\n { { { here \n{ test\n = -61 test2 = -1.2565 } ]  \n";
    FILE *f = NULL;
    fopen_s(&f,"test.txt","wb");
    sl_assert(f != NULL);
    fwrite(str,strlen(str),1,f);
    fclose(f);

    cerr << "Text in file is: \n----\n";
    cerr << str;
    cerr << "----\nParsing ... " << endl;

    BasicParser::FileStream                      s("test.txt");
    BasicParser::Parser<BasicParser::FileStream> b(s);
    cerr << "integer is: " << b.readInt() << endl;
    b.reachChar(',');
    cerr << "string is : " << b.readString() << endl;
    cerr << "string is : " << b.readString() << endl;
    b.reachChar('\"');
    cerr << "string is : " << b.readString("\"") << endl;
    cerr << "reaching 'here'" << endl;
    b.reachString("here");
    cerr << "reaching '{'" << endl;
    b.reachChar('{');
    cerr << "string is : " << b.readString() << endl;
    b.reachChar('=');
    cerr << "integer is: " << b.readInt() << endl;
    cerr << "string is : " << b.readString() << endl;
    b.reachChar('=');
    cerr << "float   is: " << b.readFloat() << endl;
    cerr << "char is   : " << char(b.readChar()) << endl;
    cerr << "char is   : " << char(b.readChar()) << endl;
    if (!b.skipSpaces()) {
      cerr << "EOF detected properly" << endl;
    }
  }

  cerr << "---------------------------" << endl;

}

// -----------

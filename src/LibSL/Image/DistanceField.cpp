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

// ------

#include <LibSL/LibSL.config.h>
#include <LibSL/Memory/Array2D.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Image/DistanceField.h>

void LibSL::Image::computeEuclidianDistanceField(LibSL::Memory::Array::Array2D<LibSL::Math::v2i>& dist)
{
	typedef LibSL::Math::v2i v2i;
	// Compute Euclidean distance map.
	//   See Per-Erik Danielsson.  Euclidean Distance Mapping.
	//   Computer Graphics and image Processing 14:227-248, 1980.
	ForRange(y,1,dist.ysize()-1) {
		ForIndex(x,dist.xsize()) {
			v2i    vt = dist.at(x,y-1);
			vt[1]    += -1;
			if (sqLength(vt) < sqLength(dist.at(x,y))) dist.at(x,y) = vt;
		}
		ForRange(x,1,dist.xsize()-1) {
			v2i    vt = dist.at(x-1,y);
			vt[0]    += -1;
			if (sqLength(vt) < sqLength(dist.at(x,y))) dist.at(x,y) = vt;
		}
		for (int x=dist.xsize()-2;x>=0;x--) {
			v2i    vt = dist.at(x+1,y);
			vt[0]    +=  1;
			if (sqLength(vt) < sqLength(dist.at(x,y))) dist.at(x,y) = vt;
		}
	}
	for (int y=dist.ysize()-2;y>=0;y--) {
		ForIndex(x,dist.xsize()) {
			v2i    vt = dist.at(x,y+1);
			vt[1]    +=  1;
			if (sqLength(vt) < sqLength(dist.at(x,y))) dist.at(x,y) = vt;
		}
		ForRange(x,1,dist.xsize()-1) {
			v2i    vt = dist.at(x-1,y);
			vt[0]    += -1;
			if (sqLength(vt) < sqLength(dist.at(x,y))) dist.at(x,y) = vt;
		}
		for (int x = dist.xsize()-2;x>=0;x--) {
			v2i    vt = dist.at(x+1,y);
			vt[0]    +=  1;
			if (sqLength(vt) < sqLength(dist.at(x,y))) dist.at(x,y) = vt;
		}
	}
}

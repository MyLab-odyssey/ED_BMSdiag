// OpenSCAD code: Arduino + Sparkfun CAN bus shield
// (C) 2018 by MyLab-odyssey
// 
// This code is based upon:
// Arduino connectors library
// Copyright (c) 2013 Kelly Egan
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do 
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

include <arduino.scad>
include <dsub.scad>
include <ED4scan-Logo.scad>

//Arduino boards
//You can create a boxed out version of a variety of boards by calling the arduino() module
//The default board for all functions is the Uno

unoDimensions = boardDimensions( UNO );

//Add a Board-Mockup if you like
/*translate( [0, 0, 8] )
	arduino(UNO);
*/

module UNO_CASE() {
    difference () {
        translate([0, 0, 0]) {
            enclosure(UNO, 3, 3, 20, 3, TAPHOLE);
        }
        translate([27.15, 80, 30]) {
            rotate([90,90,0]) {
                dsub(1.1, 18, 5);
            }
        }
    }
}

module UNO_LID() {
    translate([-20, 0, 0]) {
        difference() {
            rotate([0,180,0]) {
                enclosureLid(UNO, 3, 3, 3, false);
            }
            rotate([0,180,90]) {
                translate([-65.5,45,2.7]) LogoImprint(0.7,15);
            }
        }
        translate([2.5, 7.9, 0])
        //color("RED")
        rotate ([0,0,90])
        linear_extrude(18.5)
            polygon(points=[[9, 0], [0, 9], [0,0]], 
                    paths=[[0,1,2]]
                );
        translate([2.5, 7.9, 0])
        //color("RED")
        rotate ([0,0,90])
        linear_extrude(21)
            polygon(points=[[7.5, 0], [0, 7.5], [0,0]], 
                    paths=[[0,1,2]]
                );
        
        translate([-55.8, 7.9, 0])
        //color("RED")
        rotate ([0,0,0])
        linear_extrude(18.5)
            polygon(points=[[9, 0], [0, 9], [0,0]], 
                    paths=[[0,1,2]]
                );
        translate([-55.8, 7.9, 0])
        //color("RED")
        rotate ([0,0,0])
        linear_extrude(21)
            polygon(points=[[7.5, 0], [0, 7.5], [0,0]], 
                    paths=[[0,1,2]]
                );
    }
}

//Create the two items, comment corresponding line for single STL-export
//UNO_CASE();
UNO_LID();

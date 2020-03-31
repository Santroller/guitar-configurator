/*
"OpenScad library. Threads for screws and nuts V1" 
by ANDRES ROMAN (AKA Cuiso) September 2018

-> V1.1 23/08/2019 Change Log:
-Fixed bug. Using fullparms functions with "divs" no divider of 360 (200 for example) the thread could be incomplete. Now you can use any number of "divs" without problem.
-A small improvement. Now the diameter reductions at the beginning of the screw for easy entry are created with the same level of detail as the rest of the screw.

* Copyright 2018 ANDRES ROMAN (AKA Cuiso)
* Licensed under the Creative Commons - Attribution license.
* This software is free software, you can Share and Adapt under the terms of Attribution
(give appropriate credit).
*   
* you can visit https://creativecommons.org/licenses/by/3.0/ for more details

* This software is distributed WITHOUT ANY WARRANTY.

*/

//For use this library you can put the file threads-library-by-cuiso-v1.scad in the same directory of your proyect and put the line use <threads-library-by-cuiso-v1.scad> in your own .scad file
//You can see examples of use in file examples.scad

/*
This is a library to include threads for screws and nuts in your own designs.

This library is focused to provide simple functions with preset parameters by default,
and not to be too demanding in CPU consumption neither in previsualizacion nor in rendering

This library has been tested printing on PLA with 0.1 layer. Except for smaller diameter
threads layers greater than 0.1 should work well.
Diamaters tested between 3mm and 10mm, but you can use any diameter of your choice. 

Shape of the threads are similar (but not equal) to ISO METRIC threads.
Some tests have been done using screws and nuts printed with this library in
combination with screws and nuts of metal with acceptable results. However, they have
not been exhaustive tests.
*/

/*
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
AVAILABLE FUNCTIONS:

BASIC FUNCTIONS (USING PREDEFINED PARAMETERS):

thread_for_screw(diameter, length);
    Generates a screw thread of the diameter indicated by the "diameter" parameter and of the length indicated by "length". This function uses standard pitch values depending on the diameter specified.
    diameter: Free value, in milimeters.
    length: Free value, in milimeters.
    
    Example:
    thread_for_screw(diameter=10, length=30);

thread_for_nut(diameter, length, usrclearance=0);
    Generates a nut thread of the diameter indicated by the "diameter" parameter and of the length indicated by "length". This function uses standard pitch values depending on the diameter specified.
    The generated thread can be subtracted from any element of your design using the operator "difference()", thus obtaining a thread where a screw will fit.
    This function uses standard clearance for adjustment between screw and nut depending on the diameter specified.
    diameter: Free value, in milimeters.
    length: Free value, in milimeters.
    usrclearance: (optional) If the screw fits too tight on the nut you can specify a positive correction value (try 0.1 for example).
    If the screw fits too loose in the nut you can specify a negative correction value (try -0.1 for example).
    
    Example:
    thread_for_nut(diameter=10, length=20);
    

MORE AVAILABLE FUNCTIONS (WITH ADDITIONAL PARAMETERS):

thread_for_screw_fullparm(diameter, length, pitch, divs=50);
    Generates a screw thread of the diameter indicated by the "diameter" parameter and of the length indicated by "length", with pitch indicated by "pitch", and a resolution indicated by "divs". 
    This function allows you to specify a non-standard "pitch" value.
    diameter: Free value, in milimeters.
    length: Free value, in milimeters.
    pitch: Pitch value.
    divs: (optional) you can obtain more or less resolution with parm. Higher values for more resolution (and more rendering time). Default value is 50.
        
    Example:
    thread_for_screw_fullparm(diameter=10, length=30, pitch=2, divs=60);

thread_for_nut_fullparm(diameter, length, usrclearance=0, pitch, divs=50, entry=1);
    Generates a nut thread with additional parameters:
    diameter: Free value, in milimeters.
    length: Free value, in milimeters.
    usrclearance: (optional) If the screw fits too tight on the nut you can specify a positive correction value (try 0.1 for example).
    If the screw fits too loose in the nut you can specify a negative correction value (try -0.1 for example).
    pitch: Pitch value.
    divs: (optional) you can obtain more or less resolution with parm. Higher values for more resolution (and more rendering time). Default value is 50.
    entry: By default, the nut threads include a wider entrance area at the beginning and end to facilitate the introduction of the screw and minimize "elephant foot" problems.
    Specify 1 in entry to use these zones and 0 to not use them.
    
    Example:
    thread_for_nut_fullparm(diameter=10,length=20,usrclearance=0.1,pitch=1.5,divs=60, entry=1);
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/   

/*
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
SOME NOTES:
- At first preview(F5) of the design may appear in console a lot of messages:
"PolySet has nonplanar faces. Attempting alternate construction"
simply ignore those messages, they are not a problem.

- I have no lag problems in preview(F5) mode during the test of this library. If you are suffering lag problems in preview you can try to un-comment the "render()" al lines 254 and 261.  

- This library uses by default PITCH values: 
  DIAMETER    PITCH_VALUE
  3mm        0.5mm
  4mm        0.7mm
  5mm        0.8mm
  6mm        1.0mm
  8mm        1.25mm
 10mm        1.5mm
 ...
 24mm        3.0mm
 (complete list defined at get_std_pitch(diam))
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/


//EXAMPLE FOR CUSTOMIZER:
//1 to draw rod, 0 to not draw rod
drawrod=1; 
//1 to draw nut, 0 to not draw nut
drawnut=1;
diameter=8; 
lengthrod=15; 
lengthnut=7; 
//Example of USE, a rod SCREW:
if(drawrod==1)thread_for_screw(diameter=diameter, length=lengthrod);
//Example of USE, a FEMALE THREAD to be removed (with difference ()) from another figure:
if(drawnut==1)
translate([diameter,1.5*diameter,0]){
thread_for_nut(diameter=diameter, length=lengthnut);
}




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
module thread_for_screw(diameter, length)
{
stdpitch=get_std_pitch(diameter);
thread_for_screw_cuiso_tec(diameter,length,stdpitch,50);
echo(diameter);echo(stdpitch);
}

module thread_for_nut(diameter, length, usrclearance=0)
{
stdpitch=get_std_pitch(diameter);
stdclearance=get_std_clearance(diameter);

thread_for_nut_cuiso_tec
(diameter+stdclearance+usrclearance,length,stdpitch,50,entry=1);
    
echo(diameter);echo(stdpitch);echo(stdclearance+usrclearance);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
module thread_for_screw_fullparm(diameter, length, pitch, divs=50)
{
if(divs<20) thread_for_screw_cuiso_tec(diameter,length,pitch,20);
    else if(divs>360) thread_for_screw_cuiso_tec(diameter,length,pitch,360);
        else thread_for_screw_cuiso_tec(diameter,length,pitch,divs);
}

module thread_for_nut_fullparm(diameter, length, usrclearance=0, pitch, divs=50, entry=1)
{
stdclearance=get_std_clearance(diameter);

if(divs<20)thread_for_nut_cuiso_tec
(diameter+stdclearance+usrclearance,length,pitch,20,entry=entry);
else
if(divs>360)thread_for_nut_cuiso_tec
(diameter+stdclearance+usrclearance,length,pitch,360,entry=entry);
else
thread_for_nut_cuiso_tec
(diameter+stdclearance+usrclearance,length,pitch,divs,entry=entry);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
function get_std_pitch(diam) = 
lookup(diam,[[3,0.5],[4,0.7],[5,0.8],[6,1.0],[8,1.25],[10,1.5],
[12,1.75],[14,2.0],[16,2.0],[18,2.5],[20,2.5],[22,2.5],[24,3.0]
]);

function get_std_clearance(diam) = 
lookup(diam,[[3,0.4],[4,0.4],[5,0.5],[6,0.6],[8,0.6],[10,0.6],
[12,0.7],[14,0.7],[16,0.7],[18,0.7],[20,0.8],[22,0.8],[24,0.8]
]);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
module thread_for_screw_cuiso_tec(diameter,lenght,pitch,divdelta){
delta=360/divdelta;
intersection(){
cubointter(long=lenght,pitch=pitch,diameter=diameter,divdeltacube=divdelta);
rosca(long=lenght,p=pitch,delta=delta,diameter=diameter);
}
//#cylinder(d=diameter,h=lenght,$fn=100);
}

module thread_for_nut_cuiso_tec(diameter,lenght,pitch,divdelta,entry){
delta=360/divdelta;
translate([0,0,-0.001])
intersection(){
rosca(long=lenght+0.002,p=pitch,delta=delta,diameter=diameter,nut=1);
cubointter_nut(lenght+0.002,pitch,diameter);
}
if(entry==1){translate([0,0,-0.001])cubointter_nut_entry(lenght+0.002,pitch,diameter,divdeltacube=divdelta);}
//#cylinder(d=diameter,h=lenght,$fn=100);
}

module cubointter(long,pitch,diameter,divdeltacube)
{
longcube=long-pitch;
translate([0,0,longcube/2+pitch/2])cube([diameter,diameter,longcube],center=true);
translate([0,0,longcube+pitch/2])
cylinder(d1=diameter,d2=diameter-2*pitch*0.866,h=pitch/2, $fn=divdeltacube);
cylinder(d2=diameter,d1=diameter-2*pitch*0.866,h=pitch/2, $fn=divdeltacube);
}

module cubointter_nut(long,pitch,diameter)
{
longcube=long;
translate([0,0,longcube/2])cube([diameter,diameter,longcube],center=true);
}

module cubointter_nut_entry(long,pitch,diameter,divdeltacube)
{
longcube=long-0.6;
translate([0,0,longcube])
cylinder(d2=diameter+0.4,d1=diameter,h=0.6, $fn=divdeltacube);
cylinder(d1=diameter+0.4,d2=diameter,h=0.6, $fn=divdeltacube);
}

module rosca(long,p,delta,diameter,nut=0)
{
vueltas=round(long/p+0.5)+1;
translate([0,0,-p/4])
for(v = [0 : 1 : vueltas-1])
translate([0,0,v*p])
vuelta(p=p,delta=delta,diameter=diameter,nut=nut);
}

module vuelta(p,delta,diameter,nut)
{
medio(p=p,delta=delta,diameter=diameter,nut=nut);

translate([0,0.001,0])
rotate([0,180,180])
medio(p=p,delta=delta,diameter=diameter,nut=nut);
}

module medio(p,delta,diameter,nut)
{
if(nut==1)
//render()
hull(){
for(k = [0 : delta : 180])
translate([0,0,k*p/360])rotate([0,0,k])pieza_nut(diameter=diameter, p=p);
translate([0,0,180*p/360])rotate([0,0,180])pieza_nut(diameter=diameter, p=p);
}
else
//render()
hull(){
for(k = [0 : delta : 180])
translate([0,0,k*p/360])rotate([0,0,k])pieza(diameter=diameter, p=p);
translate([0,0,180*p/360])rotate([0,0,180])pieza(diameter=diameter, p=p);
}
}

module pieza(diameter, p)
{
r=diameter/2;
h=p*0.866;
pp=p/2;
pm=-p/2;
hh=r-h;

polyhedron([
    [0,0,pp],[hh,0,pp],[r-h/8,0,p/16],[r-h/8,0,-p/16],[hh,0,pm],[0,0,pm]
    ], 
    [[0,1,2,3,4,5]]);
}

module pieza_nut(diameter, p)
{
r=diameter/2;
h=p*0.866;
pp=p/2;
pm=-p/2;
hh=r-h;
polyhedron([[0,0,pp],[hh,0,pp],[r,0,0],[hh,0,pm],[0,0,pm]], [[0,1,2,3,4]]);
}
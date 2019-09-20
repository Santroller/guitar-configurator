scale([100,100,100]) import("cover-gh3.stl");
difference() {
    difference() {
        translate([-7,-5.5,-1.7]) scale([1,0.25,0.15]) cube(13);
        translate([-6.7,-5.2,-1.8]) scale([0.95,0.2,0.2]) cube(13);   
    }
    translate([-7.5,-4.35,-2]) scale([1,1,1.5]) cube(1);
    translate([5.5,-4.35,-2]) scale([1,1,1.5]) cube(1);
}

nozzle_size = 0.4;
hook_thickness = 3*nozzle_size;
module hook() {
    linear_extrude(1) polygon(points=[[0,0],[2*hook_thickness,0],[hook_thickness,hook_thickness]]);
}
difference() {
    translate([-7,2,-0.4]) scale([0.95,0.2,0.04]) cube(13);
    translate([-3.5,2,0]) scale([7,0.1,0.1]) rotate([90,90,90]) hook();
    translate([3.5,4.6,0]) scale([7,0.1,0.1]) rotate([-90,90,90]) hook();
}
translate([-3.5,-5.2,-1.35]) scale([7,0.1,0.1]) rotate([90,90,90]) hook();
translate([3.5,-2.6,-1.35]) scale([7,0.1,0.1]) rotate([-90,90,90]) hook();
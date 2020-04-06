scale([1000, 1000, 1000]) import("cover-gh3.stl");
difference() {
    difference() {
        translate([-70,-55,-17]) scale([10,2.5,1.5]) cube(13);
        translate([-67,-52,-18]) scale([9.5,2,2]) cube(13);   
    }
    translate([-75,-43.5,-20]) scale([10,10,15]) cube(1);
    translate([55,-43.5,-20]) scale([10,10,15]) cube(1);
}

nozzle_size = 0.4;
hook_thickness = 3*nozzle_size;
module hook() {
    linear_extrude(1) polygon(points=[[0,0],[2*hook_thickness,0],[hook_thickness,hook_thickness]]);
}
difference() {
    translate([-70,10,-4]) scale([9.5,2,0.4]) cube(13);
    translate([30,36,0]) scale([70,1,1]) rotate([-90,90,90]) hook();
    translate([-40,10,0]) scale([70,1,1]) rotate([90,90,90]) hook();
}
translate([-40,-52,-13.5]) scale([70,1,1]) rotate([90,90,90]) hook();
translate([30,-26,-13.5]) scale([70,1,1]) rotate([-90,90,90]) hook();
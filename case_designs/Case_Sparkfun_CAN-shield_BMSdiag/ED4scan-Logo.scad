// OpenSCAD code: ED4scan logo
// (C) 2018 by MyLab-odyssey

letter_size = 15;

module LogoImprint(lHeight = 5, lSize = 50) {
linear_extrude(height = lHeight) {
		text("E", size = letter_size, font = "Phosphate:style=Solid", halign = "center", valign = "center", $fn = 20);
}

translate ([letter_size * 0.28 ,0,0])
linear_extrude(height = lHeight)
		hull() text("D", size = letter_size, font = "Phosphate:style=Solid", valign = "center", $fn = 20);

translate ([letter_size * 1.0,-3,0])
linear_extrude(height = lHeight)
		text(" BMSdiag", size = letter_size*0.575, font = "Arial:style=bold", valign = "center", $fn = 20);
}

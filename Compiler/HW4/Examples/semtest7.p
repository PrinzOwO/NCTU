//&S-
//&T-
//&D-
semtest7;
begin
var aconst : 10;
var a: array 1 to 3 of array 1 to 3 of integer;
var b: array 1 to 3 of array 1 to 3 of integer;
a := b;
a[1] := b[2];
a[1][1] := b[1][10];
aconst := 5;
end
end semtest7

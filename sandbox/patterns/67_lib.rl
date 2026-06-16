/* Pattern 67 library — two machine definitions at top level */
%%{ machine lib1;
action L1A { printf(" lib1_a"); }
action L1B { printf(" lib1_b"); }
a1 = 'A' @L1A;
b1 = 'B' @L1B;
}%%

%%{ machine lib2;
action L2C { printf(" lib2_c"); }
action L2D { printf(" lib2_d"); }
c2 = 'C' @L2C;
d2 = 'D' @L2D;
}%%

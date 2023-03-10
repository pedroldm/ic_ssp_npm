param m; # Quantidade de máquinas
param j; # Quantidade de tarefas
param t; # Quantidade de ferramentas

set M := {1 .. m}; # Set de máquinas
set J := {1 .. j}; # Set de tarefas
set T := {1 .. t}; # Set de ferramentas
set JT {T};

param c{i in M}; # Capacidade do magazine de cada máquina
param p{J cross M}; # Tempo de completude das tarefas em cada máquina
param sw{i in M}; # Tempo gasto por troca em cada máquina
param G;

var f{J, J, M};
var x{J, J, M}, binary;
var w{T, J, M}, binary;
var v{T, J, M}, binary;

minimize tse : sum {ti in T, r in J, mi in M} w[ti, r, mi];

r1 {ji in J} : sum {r in J, mi in M} x[ji,r,mi] = 1;
r2 {r in J, mi in M} : sum {ji in J} x[ji,r,mi] <= 1;
r3 {r in J diff {1}, mi in M} : sum {ji in J} x[ji, r, mi] <= sum {ji in J} x[ji, r - 1, mi];
r4 {ti in T, r in J, mi in M} : sum {ji in JT[ti]} x[ji, r, mi] <= v[ti, r, mi];
r5 {r in J, mi in M} : sum {ti in T} v[ti, r, mi] <= c[mi];
r6 {ti in T, r in J diff {1}, mi in M} : v[ti, r, mi] - v[ti, r-1, mi] <= w[ti, r, mi];
r7 {ji in J, mi in M} : f[ji, 1, mi] = p[ji, mi] * x[ji, 1, mi];
r8 {ji in J, r in J diff {1}, mi in M} : f[ji, r, mi] >= sum {i in J : i != ji} f[i, r - 1, mi] + sw[mi] * sum {ti in T} w[ti, r, mi] + p[ji, mi] * x[ji, r, mi] - G * (1-x[ji, r, mi]);

solve;

display x;

data;
param m := 2;
param j := 10;
param t := 10;
param c := 1 5 2 7;
set JT[1] := 3, 5, 8, 10;
set JT[2] := 1, 2, 5, 7, 8;
set JT[3] := 3, 4, 6, 7, 8, 9;
set JT[4] := 1, 4, 9, 10;
set JT[5] := 2, 3, 6, 7, 8, 9;
set JT[6] := 9, 10;
set JT[7] := 2, 3, 5, 6, 8;
set JT[8] := 1, 10;
set JT[9] := 3, 4, 6, 7, 10;
set JT[10] := 2, 6, 9;
param p :
	1	2:=
	1	2	8
	2	7	10
	3	2	2
	4	5	1
	5	10	2
	6	8	7
	7	10	5
	8	8	3
	9	8	10
	10	9	8;
param sw := 1 2 2 4;
param G := 9999999;

end;

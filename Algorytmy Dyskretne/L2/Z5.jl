using JuMP
import GLPK
using JSON

P=1:3
Z=1:3
MinRadiowozy=[2 4 3; 3 6 5; 5 7 6]#[p,z]
MaxRadiowozy=[3 7 5; 5 7 10; 8 12 10]#[p,z]
DostepneRadiowozy=[10 20 18]#[z]
Powinno=[10 14 13]#[p] conajmniej
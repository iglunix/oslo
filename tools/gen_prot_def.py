#!/usr/bin/env python3

def deffunc(ofile, namelist, name, ret, params):
	ofile.write(ret + " (")
	ofile.write("efi_func *" + name + ")(")
	for i in namelist:
		ofile.write(params[i] + " " + i)
		if i != namelist[len(namelist) - 1]:
			ofile.write(", ");
	ofile.write(");\n")

protname = input("Type in protocol name: ")
if protname == "":
	print("Invalid protocol name! Exiting..")
	exit(1)

protname = "efi_" + protname + "_protocol"

ofile = open(protname + ".h", "w")
# Create header file
ofile.write("#ifndef __" + protname.upper() + "_H\n")
ofile.write("#define __" + protname.upper() + "_H\n\n")

ofile.write("typedef struct _" + protname + " " + protname + "_t;\n")
ofile.write("struct _" + protname + " {\n")

while True:
	l = input("Type in function name: ")
	if l == "":
		break
	ret = input("Type in function return type (enter = efi_status_t): ")
	if ret == "":
		ret = "efi_status_t"
	args = dict()
	namelist = list()
	while True:
		argname = input("Type in argument name: ")
		if argname == "":
			break
		elif argname == "self":
			argtype = protname + "_t*"
		else:
			argtype = input("Type in argument type: ")
		namelist.append(argname)
		args[argname] = argtype
	deffunc(ofile, namelist, l, ret, args)

ofile.write("};\n\n");

# Finish header file
ofile.write("#endif\n")
ofile.close()

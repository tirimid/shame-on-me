import os
import sys

def genmodel(infile, outfile):
	flines = None
	with open(infile) as f:
		flines = f.readlines()

	v = []
	vn = []
	vt = []
	f = []

	for ln in flines:
		if len(ln) < 2:
			continue

		type = ln[0:2].strip()
		match type:
			case "v":
				newv = ln[2:].split()
				v.append((float(newv[0]), float(newv[1]), float(newv[2])))
			case "vn":
				newvn = ln[3:].split()
				vn.append((float(newvn[0]), float(newvn[1]), float(newvn[2])))
			case "vt":
				newvt = ln[3:].split()
				vt.append((float(newvt[0]), float(newvt[1])))
			case "f":
				newf = ln[2:].split()

				newf0 = newf[0].split("/")
				newf1 = newf[1].split("/")
				newf2 = newf[2].split("/")

				newf0 = (int(newf0[0]) - 1, int(newf0[1]) - 1, int(newf0[2]) - 1)
				newf1 = (int(newf1[0]) - 1, int(newf1[1]) - 1, int(newf1[2]) - 1)
				newf2 = (int(newf2[0]) - 1, int(newf2[1]) - 1, int(newf2[2]) - 1)

				f.append((newf0, newf1, newf2))
			case _:
				pass

	# make into correct vertex format.
	verts = []
	for i in f:
		for j in i:
			pos = v[j[0]]
			texcoord = vt[j[1]]
			norm = vn[j[2]]
			vert = (pos[0], pos[1], pos[2], texcoord[0], texcoord[1], norm[0], norm[1], norm[2])
			verts.append(vert)
	verts = list(set(verts))

	# make into correct index format.
	inds = []
	for i in f:
		for j in i:
			pos = v[j[0]]
			texcoord = vt[j[1]]
			norm = vn[j[2]]
			vert = (pos[0], pos[1], pos[2], texcoord[0], texcoord[1], norm[0], norm[1], norm[2])
			inds.append(verts.index(vert))

	fname = os.path.splitext(os.path.basename(outfile))[0]
	out = [
		f"#define {fname}_VERT_CNT {len(verts)}\n",
		f"#define {fname}_IDX_CNT {len(inds)}\n",
		f"static float const {fname}_VertData[] =\n",
		"{\n",
	]

	for i in verts:
		out.append(f"\t{i[0]}, {i[1]}, {i[2]}, {i[3]}, {i[4]}, {i[5]}, {i[6]}, {i[7]},\n")

	out.append("};\n")
	out.append(f"static unsigned const {fname}_IdxData[] =\n")
	out.append("{\n")

	for i in range(len(inds) // 3):
		out.append(f"\t{inds[3 * i]}, {inds[3 * i + 1]}, {inds[3 * i + 2]},\n")

	out.append("};\n")

	with open(outfile, "w") as f:
		f.writelines(out)

needargs = {
	"-m": 4,
}

if sys.argv[1] not in needargs:
	sys.stderr.write("bad gentype, must be -m!\n")
	quit(1)

if len(sys.argv) != needargs[sys.argv[1]]:
	sys.stderr.write("wrong argument count for gentype!\n")
	quit(1)

match sys.argv[1]:
	case "-m":
		genmodel(sys.argv[2], sys.argv[3])

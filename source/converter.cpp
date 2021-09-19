#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

struct Dim {
    int width;
    int height;
    int depth;

    int total() {
        return width * height * depth;
    }

    int index(int x, int y, int z) {
        return width * height * z + width * y + x;
    }
};

class Sample {
    public:
        float x;
        float y;
        float z;

        void set(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        void add(Sample &v) {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        void normalize() {
            float len = std::sqrt(x * x + y * y + z * z);
            if (len == 0.0)
                return;
            x = x / len;
            y = y / len;
            z = z / len;
        }
};

class Voxel : public Sample {
    public:
        float a;

        Voxel () {
            x = y = z = a = 0.0f;
        }

        void set(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        void set(float x, float y, float z, float a) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->a = a;
        }

        void set(Sample &s) {
            this->x = s.x;
            this->y = s.y;
            this->z = s.z;
        }

        void output(std::ofstream &out) {
            out.put((unsigned char) std::floor(254.0f * x));
            out.put((unsigned char) std::floor(254.0f * y));
            out.put((unsigned char) std::floor(254.0f * z));
            out.put((unsigned char) std::floor(254.0f * a));
        }
};

bool parseOptions(int argc,
                  const char *argv[],
                  std::string &inName,
                  std::string &outName,
                  Dim &dimensions,
                  int &gradient,
                  int &sample);
void fillVoxels(Voxel *voxels, unsigned char *raw, Dim &dim);
void fillGradients(Voxel *voxels, Dim &dim, int N);
void calcGradient(Voxel *voxels, Dim &dim,
                  int x, int y, int z,
                  int delta0, int delta1);

void sampleGradients(Voxel *voxels, Dim &dim, int N);
void calcAverage(Sample &sample, Voxel *voxels, Dim &dim,
                 int x,
                 int y,
                 int z,
                 int N);

int main(int argc, const char *argv[])
{
    std::string inName;
    std::string outName;
    Dim dim;

    int gradientN;
    int sampleN;

    if(!parseOptions(argc, argv, inName, outName, dim, gradientN, sampleN)) {
        std::cerr << "Error on parsing arguments..." << std::endl;
        return 0;
    }

    unsigned char* inputData = new unsigned char[dim.total()];

    std::ifstream in(inName.c_str(), std::ios::binary);
    in.read((char*)(inputData), sizeof(unsigned char) * dim.total());
    in.close();


    Voxel *voxels = new Voxel[dim.total()];
    fillVoxels(voxels, inputData, dim);
    delete []inputData;

    fillGradients(voxels, dim, gradientN);
    sampleGradients(voxels, dim, sampleN);
    sampleGradients(voxels, dim, sampleN);

    std::ofstream out(outName.c_str(), std::ios::binary);
    for (int z = 0; z < dim.depth; ++z) {
        for (int y = 0; y < dim.height; ++y) {
            for (int x = 0; x < dim.width; ++x) {
                voxels[ dim.index(x, y, z) ].output(out);
            }
        }
    }
    out.close();
    delete []voxels;

    return 0;
}

void fillVoxels(Voxel *voxels, unsigned char *raw, Dim &dim)
{
    for (int z = 0; z < dim.depth; ++z) {
        for (int y = 0; y < dim.height; ++y) {
            for (int x = 0; x < dim.width; ++x) {
                int index =  dim.index(x, y, z);
                voxels[index].set(0.0f, 0.0f, 0.0f, raw[index] / 256.0f);
            }
        }
    }
}

void fillGradients(Voxel *voxels, Dim &dim, int N)
{
    //for (unsigned int z = 0; z < N; ++z)
        //for (unsigned int y = 0; y < N; ++y)
            //for (unsigned int x = 0; x < N; ++x)
                //calcGradient(voxels, dim, x, y, z, 0, N);

    for (int z = 0; z < dim.depth; ++z)
        for (int y = 0; y < dim.height; ++y)
            for (int x = 0; x < dim.width; ++x)
                calcGradient(voxels, dim, x, y, z, N, -N);

    //for (unsigned int z = dim.depth - N; z < dim.depth; ++z)
        //for (unsigned int y = dim.height - N; y < dim.height; ++y)
            //for (unsigned int x = dim.width - N; x < dim.width; ++x)
                //calcGradient(voxels, dim, x, y, z, -N, 0);
}

void calcGradient(Voxel *voxels, Dim &dim,
                  int x, int y, int z,
                  int delta0, int delta1)
{
    if (
        x + delta0 >= dim.width ||
        x + delta1 >= dim.width ||

        y + delta0 >= dim.height ||
        y + delta1 >= dim.height ||

        z + delta0 >= dim.depth ||
        z + delta1 >= dim.depth ||

        x - delta0 < 0 ||
        x - delta1 < 0 ||

        y - delta0 < 0 ||
        y - delta1 < 0 ||

        z - delta0 < 0 ||
        z - delta1 < 0
    ) {
        voxels[ dim.index(x, y, z) ].set(0.0, 1.0, 0.0);
        return;
    }

    float s1X = voxels[ dim.index(x + delta0, y, z) ].a;
    float s1Y = voxels[ dim.index(x, y + delta0, z) ].a;
    float s1Z = voxels[ dim.index(x, y, z + delta0) ].a;

    float s2X = voxels[ dim.index(x + delta1, y, z) ].a;
    float s2Y = voxels[ dim.index(x, y + delta1, z) ].a;
    float s2Z = voxels[ dim.index(x, y, z + delta1) ].a;

    float sX = s2X - s1X;
    float sY = s2Y - s1Y;
    float sZ = s2Z - s1Z;

    float len = std::sqrt(sX * sX + sY * sY + sZ * sZ);
    if (len == 0.0)
        len = 1.0;

    voxels[ dim.index(x, y, z) ].set(sX / len, sY / len, sZ / len);
}

void sampleGradients(Voxel *voxels, Dim &dim, int N)
{
    Sample *samples = new Sample[dim.total()];

    int index = 0;
    for (int z = 0; z < dim.depth; ++z) {
        for (int y = 0; y < dim.height; ++y) {
            for (int x = 0; x < dim.width; ++x) {
                calcAverage(samples[index++], voxels, dim, x, y, z, N);
            }
        }
    }

    index = 0;
    for (int z = 0; z < dim.depth; ++z) {
        for (int y = 0; y < dim.height; ++y) {
            for (int x = 0; x < dim.width; ++x) {
                voxels[ dim.index(x, y, z) ].set(samples[index++]);
            }
        }
    }

    delete []samples;
}

void calcAverage(Sample &sample, Voxel *voxels, Dim &dim,
                 int x,
                 int y,
                 int z,
                 int N)
{
    sample.set(0.0f, 0.0f, 0.0f);

    sample.add(voxels[ dim.index(x, y, z) ]);

    int count = 0;
    for (int k = z - N; k <= z + N; ++k) {
        if (k >= dim.depth || k < 0)
            continue;
        for (int j = y - N; j <= y + N; ++j) {
            if (j >= dim.height || j < 0)
                continue;
            for (int i = x - N; i <= x + N; ++i) {
                if (i >= dim.width || i < 0)
                    continue;
                count++;
                sample.add(voxels[ dim.index(i, j, k) ]);
            }
        }
    }

    sample.x /= count;
    sample.y /= count;
    sample.z /= count;
    sample.normalize();
}

bool parseOptions(int argc,
                  const char *argv[],
                  std::string &inName,
                  std::string &outName,
                  Dim &dim,
                  int &gradient,
                  int &sample)
{
    if(argc < 5)
        return false;

    inName.assign(argv[1]);
    std::ifstream in(inName.c_str(), std::ios::binary);
    if(!in) {
        in.close();
        return false;
    }
    in.seekg(0, std::ios::end);
    int fileSize = in.tellg();
    in.close();

    if (argc >= 8) {
        std::string temp(argv[2]);
        temp.append(" "); temp.append(argv[3]);
        temp.append(" "); temp.append(argv[4]);
        temp.append(" "); temp.append(argv[6]); // gradient
        temp.append(" "); temp.append(argv[7]); // sample

        std::istringstream iss(temp);

        iss >> dim.width >> dim.height >> dim.depth >> gradient >> sample;

        if (fileSize != dim.total())
            return false;
        outName.assign(argv[5]);
    } else {
        int sideSize = std::ceil(std::pow(fileSize, 1.0 / 3.0));
        if (sideSize * sideSize * sideSize != fileSize)
            return false;
        dim.width = dim.height = dim.depth = sideSize;
        outName.assign(argv[2]);

        std::istringstream iss(std::string(argv[3]) + " " + std::string(argv[4]));
        iss >> gradient >> sample;
    }

    return true;
}


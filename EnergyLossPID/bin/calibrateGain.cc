#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "../interface/EstimateHandler.h"
#include "../interface/SlimMeasurement.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;

typedef std::pair<uint32_t, unsigned char> ChipId;

map<ChipId, vector<SlimMeasurement> > hits;
map<uint32_t, float> thickness;

int nchunks, pass;

typedef std::pair<uint32_t, unsigned char> ChipId;
std::map<ChipId, float> gain;

/*****************************************************************************/
void options(int arg, char **arc)
{
  int i = 1;

  do
  {
    if(strcmp(arc[i],"-nchunks") == 0) nchunks = atoi(arc[++i]);
    if(strcmp(arc[i],"-pass")    == 0) pass    = atoi(arc[++i]);

    i++;
  }
  while(i < arg);
}

/*****************************************************************************/
void readPreviousGainCorrection()
{
  cerr << " reading gain from";

  char fileName[256];
  sprintf(fileName,"UserCode/EnergyLossPID/data/gain_%d.dat",pass-1);

  cerr << " gain_" << pass-1 << ".dat";

  edm::FileInPath fileInPath(fileName);
  ifstream fileGain(fileInPath.fullPath().c_str());

  int i = 0;
  while(!fileGain.eof())
  {
    uint32_t det;
    int chip;

    int d;
    float g,f;
    string s;

    fileGain >> hex >> det;
    fileGain >> dec >> chip;
    ChipId detId(det, (unsigned char)chip);

    fileGain >> dec >> d;
    fileGain >> g; fileGain >> f;
    fileGain >> s;

    if(!fileGain.eof())
    {
      if(g > 0.5 && g < 2.0) gain[detId] = g;
                        else gain[detId] = -1.;
    }

    if(i++ % 5000 == 0) cerr << ".";
  }

  fileGain.close();

  cerr << " [done]" << endl;
}

/*****************************************************************************/
int main(int arg, char **arc)
{
  cerr << "\033[22;31m" << "Calibrating chip gains..."
       << "\033[22;0m"  << endl;

  options(arg,arc);

  if(pass == 1)
   readPreviousGainCorrection();

  char fileName[256];

  // Read maps
  for(int j = 1 ; j <= nchunks; j++)
  {
    sprintf(fileName,"../out/hits_%d.bin", j);
    cerr << "  reading.. " << fileName << endl;
    ifstream inFile(fileName, ios::binary);

    // hits
    {
      int size; inFile.read((char *)&size, sizeof(size));

      for(int i = 0; i < size; ++i)
      {
        ChipId key; inFile.read((char *)&key, sizeof(key));
        int n;    ; inFile.read((char *)&n  , sizeof(n  ));

        vector<SlimMeasurement> value;
        for(int k = 0; k < n; k++)
        {
          SlimMeasurement slim;
          inFile.read((char *)&slim, sizeof(slim));
          value.push_back(slim);
        }

        hits[key].insert(hits[key].end(), value.begin(), value.end() );
      }
    }

    // thickness 
    {
      int size; inFile.read((char *)&size, sizeof(size));

      for(int i = 0; i < size; ++i)
      {
        uint32_t key; inFile.read((char *)&key,   sizeof(key));
        float value ; inFile.read((char *)&value, sizeof(value));
        thickness[key] = value;
      }
    }
  }

  cerr << " all read" << endl;

  EstimateHandler * estimateHandler = new EstimateHandler();

  sprintf(fileName,"../data/gain_%d.dat",pass);
  cerr << " writing to " << fileName << "..." << endl;
  estimateHandler->calibrateGain(hits, thickness, fileName, gain);

  return 0;
}


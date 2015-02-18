#ifndef _Enumerators_h_
#define _Enumerators_h_

#include <cstring>
#include <vector>

enum AllDetector { PXB=0, PXF=1, TIB=2, TID=3, TOB=4, TEC3=5, TEC5=6, nDets };
static std::string detNames[nDets] =
                 {"PXB", "PXF", "TIB", "TID", "TOB", "TEC3", "TEC5"};

enum { eta=0,               nVars=1    };
enum { pos=0, neg=1,        nCharges=2 };

enum { unknown=0, elec=1, pion=2, kaon=3, prot=4, nParts=5 };
enum {                                            nConds=5 };
enum {                                            mixed =5 };

static std::string varNames[nVars]       = {"eta"};
static std::string chargeNames[nCharges] = {"pos", "neg"};

static std::string partNames[nParts] = {"unknown","elec","pion","kaon","prot"};

static double mass[nParts] =
     {0, 0.511e-3, 0.139570, 0.493677, 0.938272}; // GeV

/*
enum Particles
{
  pip, pim, kap, kam,
  prp, prm, elp, elm,
  gam, k0s, lam, ala,
  kst, aks,
  sip, asi,
  any, piz, nParticles
};

static std::string particleNames[nParticles] =
{
  "pip", "pim", "kap", "kam",
  "prp", "prm", "elp", "elm",
  "gam", "k0s", "lam", "ala",
  "kst", "aks",
  "sip", "asi",
  "any", "piz"
};

#define nFeedDowns 16
static std::pair<int,int> feedDown[nFeedDowns] =
{
  std::pair<int,int>(k0s, pip), std::pair<int,int>(k0s, pim),
  std::pair<int,int>(lam, prp), std::pair<int,int>(lam, pim),
  std::pair<int,int>(ala, prm), std::pair<int,int>(ala, pip),
  std::pair<int,int>(sip, prp), std::pair<int,int>(asi, prm),
  std::pair<int,int>(any, pip), std::pair<int,int>(any, pim),
  std::pair<int,int>(any, kap), std::pair<int,int>(any, kam),
  std::pair<int,int>(any, prp), std::pair<int,int>(any, prm),
  std::pair<int,int>(any, elp), std::pair<int,int>(any, elm)
};

*/

#endif

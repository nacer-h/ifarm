#ifndef _DCdEdxInformation_
#define _DCdEdxInformation_

#include <TObject.h>

class DCdEdxInformation : public TObject{

 public:

  DCdEdxInformation(){};
  ~DCdEdxInformation(){};

  // Data members
  double dBeta;
  double dMomentum;
  double dTheta;
  double dVertexZ;

  double ddEdx_FDC;
  double ddx_FDC;
  unsigned int dNumHitsUsedFordEdx_FDC;
  double ddEdx_CDC;
  double ddx_CDC;
  unsigned int dNumHitsUsedFordEdx_CDC;

  double dChiSq_DCdEdx;
  unsigned int dNDF_DCdEdx;
  double dFOM;

 private:
  ClassDef(DCdEdxInformation, 1);

};

#endif // _DCdEdxInformation_

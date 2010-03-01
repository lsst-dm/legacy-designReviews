// -*- lsst-c++ -*-
#include <iostream>

#include "Schema.h"
#include "Measurement.h"
#include "Photometry.h"
#include "Image.h"

#include "AperturePhotometry.h"

/************************************************************************************************************/
//
// Usage: ./main type [type ...]  where type is one of "aper", "psf", and "model"
//
int main(int argc, char **argv) {
    // Create our measuring object based on argv
    MeasurePhotometry *w = new MeasurePhotometry();
    
    for (int i = 1; i != argc; ++i) {
        w->addAlgorithm(argv[i]);
    }
    // Measure the data and retrieve the answers
    Image im = 1.0;
    MeasurePhotometry::Values v = w->measure(im);

    im = 10;
    MeasurePhotometry::Values v2 = w->measure(im);

    std::cout << v << std::endl;
    std::cout << v2 << std::endl;
    std::cout << std::endl;
    //
    // Subclasses of Photometry need a cast to use the accessors that aren't in the base class
    // (This doesn't compile if you don't include a definition of AperturePhotometry)
    //
    // Note that we can use get() to return a value as a double given its name;  the accessor
    // functions can use a templated version of get to return the true type
    //
    Photometry::Ptr photom = *v.begin();
    std::cout << "flux: " << photom->getFlux() << " fluxErr: " << photom->get("fluxErr");
#if defined(APERTURE_PHOTOMETRY_H)
    {
        AperturePhotometry::Ptr aphotom = boost::dynamic_pointer_cast<AperturePhotometry>(photom);

        if (aphotom) {
            std::cout << "  Radius: " << aphotom->getRadius(0);
        }
    }
    std::cout << std::endl << std::endl;
#endif
    //
    // Return everything using the schema
    //
    for (Measurement<Photometry>::const_iterator vptr = v.begin(); vptr != v.end(); ++vptr) {
        Schema const& sch = *(*vptr)->getSchema();

        for (Schema::const_iterator sptr = sch.begin(); sptr != sch.end(); ++sptr) {
            Schema const& se = *(*sptr);

            std::cout << sch.getComponent() << "." << se.getName() << "\t:    " << se.getType() << "\t|   ";
            if (se.isArray()) {
                for (int i = 0; i != se.getDimen(); ++i) {
                    std::cout << (se.getType() == Schema::INT ?
                                  (*vptr)->getAsLong(i, **sptr) : (*vptr)->get(i, **sptr));
                    if (i + 1 != se.getDimen()) {
                        std::cout << " ";
                    }
                }
            } else {
                std::cout << (se.getType() == Schema::INT ? (*vptr)->getAsLong(**sptr) : (*vptr)->get(**sptr));
            }
            if (se.getUnits() != "") {
                std::cout << "\t" << se.getUnits();
            }
            std::cout << std::endl;
        }
    }
}
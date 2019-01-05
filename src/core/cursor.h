#pragma once

#include "pos2d.h"


namespace teditor {

/** Represents start/end of regions in a buffer */
class Regions: public Positions {
public:
    /** mark region start/end locations as specified in the input */
    void enable(const Positions& p);

    /**
     * @defgroup InsideCheck Check if the given location is in the regions
     * @{
     */
    bool isInside(int y, int x, const Positions& cu) const;
    bool isInside(int y, int x, const Positions& cu, int i) const;
    /** @} */

    /** get all region start/end markers */
    const Positions& getLocs() const { return *this; }
};

}; // end namespace teditor

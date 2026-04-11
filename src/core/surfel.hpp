#include "primitive.hpp"
#include "geometry.hpp"

class Surfel{
	public:
		Surfel( const Point&p,const Vector&n, const Vector&wo, float time,
			const Point2f& uv, const Primitive *pri )
			: p{p}, n{n}, wo{wo}, time{time}, uv{uv}, primitive{pri}
            {/* empty */};

		Point p;        //!< Contact point.
		Vector n;       //!< The surface normal.
		Vector wo;      //!< Outgoing direction of light, which is -ray.
		Point2f uv;     //!< Parametric coordinate (u,v) of the hit surface.
		const Primitive *primitive=nullptr; //!< Pointer to the primitive.
};

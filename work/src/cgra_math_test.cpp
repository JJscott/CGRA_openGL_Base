
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <random>

#include "cgra_math.hpp"


using namespace std;
using namespace cgra;


int w_ = 4, total = 1000;
float EPSILON = 0.001;



// Square error metrics
//
template<typename T>
auto vsqerr(const T &a, const T &b) {
	T c = a - b;
	return dot(c, c);
}


template<typename T, typename U>
void testMath() {
	unordered_map<string, uint> m;
	auto test = [&](T a, T b, string s, bool f = true) {
		if ((vsqerr(a, b) < U(EPSILON)) != f)
			m[s] = m[s]+1;
	};

	for (int i = 0; i < total; ++i) {

		T a = T::random();
		U av = math::random<U>();
		T b = T::random();
		U bv = math::random<U>();
		T c = T::random();
		U cv = math::random<U>();
		T d = T::random();
		U dv = math::random<U>();
		T z = T();
		U zv = U(0);
		T o = T(1);
		U ov = U(1);

		test(-(-a), a,			"negation (double negation)");

		test(a+b, b+a,			"addition (commutativity)");
		test(a+(b+c), (a+b)+c,	"addition (associativity)");
		test(a+z, a,			"addition (identity)");

		test(a-b, -(b-a),		"subtraction (anticommutativity)");
		test(a-(b-c), (a-b)-c,	"subtraction (nonassociativity)", false);
		test(a-z, a,			"subtraction (identity)");
		test(z-a, -a,			"subtraction (negation)");

		test(a*b, b*a,				"multiplication (commutativity)");
		test(a*(b*c), (a*b)*c,		"multiplication (associativity)");
		test(a*(av+bv), a*av+a*bv,	"multiplication (distributive)");
		test(a*o, a,				"multiplication (identity)");
		test(a*z, z,				"multiplication (zero property)");
		test(-o*-o*a, a,			"multiplication (negation)");
		test(a*(o/a), o,			"multiplication (inverse)");

		test((a+b)/c, a/c + b/c,	"division (right distributive)");
		test(a/(b+c), a/b + a/c,	"division (left antidistributive)", false);


		// Angle and Trigonometry Functions
		// 

		test(degrees(radians(a)), a, "degrees / radians");
		test(radians(degrees(a)), a, "radians / degrees");

		test(asin(sin(a)), a, "trigonometry (sin double inverse)");
		test(acos(cos(a)), a, "trigonometry (cos double inverse)");
		test(atan(tan(a)), a, "trigonometry (tan double inverse)");

		test(tan(a), sin(a)/cos(a),				"trigonometry (quotient)");
		test(o/tan(a), (o/sin(a))/(o/cos(a)),	"trigonometry (inverse quotient)");

		test(sin(a)*sin(a) + cos(a)*cos(a), o, "trigonometry (sin-cos pythagorean)");
		test(o+tan(a)*tan(a), (o/cos(a))*(o/cos(a)), "trigonometry (tan-sec pythagorean)");
		test(o+(o/tan(a))*(o/tan(a)), (o/sin(a))*(o/sin(a)), "trigonometry (tan-sec pythagorean)");


		// Exponential Functions
		// 
		test(a*a, pow(a, T(2)), "pow (double, equality)");
		test(a*a*a, pow(a, T(3)), "pow (triple, equality)");

		test(log(exp(a)), a, "exp / log (double inverse)");
		test(exp(log(a)), a, "exp / log (double inverse, reverse)");

		test(log2(exp2(a)), a, "exp2 / log2 (double inverse)");
		test(exp2(log2(a)), a, "exp2 / log2 (double inverse, reverse)");

		test(sqrt(pow(a, T(2))), a, "pow / sqrt (double inverse)");
		test(1/sqrt(a), inversesqrt(a), "sqrt / inversesqrt (equality)");




	}

	for (auto p : m) {
		cout << setw(w_) << (100 * p.second / float (total)) << "% error : " << p.first << endl;
	}
}




template<typename T, typename U>
void testVectorCompile() {
	U rv = math::random<U>();
	T r;
	r = T(rv);
	r = -r;
	r += T::random();
	r += math::random<U>();
	r -= T::random();
	r -= math::random<U>();
	r *= T::random();
	r *= math::random<U>();
	r /= T::random();
	r /= math::random<U>();

	r = T::random() + T::random();
	r = math::random<U>() + T::random();
	r = T::random() + math::random<U>();
	r = T::random() - T::random();
	r = math::random<U>() - T::random();
	r = T::random() - math::random<U>();
	r = T::random() * T::random();
	r = math::random<U>() * T::random();
	r = T::random() * math::random<U>();
	r = T::random() / T::random();
	r = math::random<U>() / T::random();
	r = T::random() / math::random<U>();

	r = radians(T::random());
	r = degrees(T::random());
	r = sin(T::random());
	r = cos(T::random());
	r = tan(T::random());
	r = asin(T::random());
	r = acos(T::random());
	r = atan(T::random(), T::random());
	r = atan(T::random());

	r = pow(T::random(), T::random());
	r = exp(T::random());
	r = log(T::random());
	r = exp2(T::random());
	r = log2(T::random());
	r = sqrt(T::random());
	r = inversesqrt(T::random());

	r = abs(T::random());
	r = sign(T::random());
	r = floor(T::random());
	r = ceil(T::random());
	r = fract(T::random());

	r = mod(T::random(), T::random());
	r = mod(T::random(), math::random<U>());
	r = cgra::min(T::random(), T::random());
	r = min(T::random(), math::random<U>());
	r = cgra::max(T::random(), T::random());
	r = max(T::random(), math::random<U>());
	r = clamp(T::random(), T::random(), T::random());
	r = clamp(T::random(), T::random(), math::random<U>());
	r = mix(T::random(), T::random(), T::random());
	r = mix(T::random(), T::random(), math::random<U>());
	r = step(T::random(), T::random());
	r = step(math::random<U>(), T::random());
	r = smoothstep(T::random(), T::random(), T::random());
	r = smoothstep(T::random(), T::random(), math::random<U>());

	isnan(r);
	isinf(r);

	rv = length(T::random());
	rv = distance(T::random(), T::random());
	rv = dot(T::random(), T::random());
	r = normalize(T::random());
	r = faceforward(T::random(), T::random(), T::random());
	r = reflect(T::random(), T::random());
	r = refract(T::random(), T::random(), math::random<U>());

	auto b = lessThan(T::random(), T::random());
	lessThanEqual(T::random(), T::random());
	greaterThan(T::random(), T::random());
	greaterThanEqual(T::random(), T::random());
	equal(T::random(), T::random());
	notEqual(T::random(), T::random());
	any(b);
	all(b);

}

// testFloatingPointVector() {

// }

// testIntegerVector() {
	
// }

// testBooleanVector() {
	
// }

template<typename T>
void testAllVectorCompile() {
	// testVectorCompile<vector2<T>, T>();
	// testVectorCompile<vector3<T>, T>();
	// testVectorCompile<vector4<T>, T>();
	cout << " == Vector2 == " << endl;
	testMath<vector2<T>, T>();
	// cout << " == Vector3 == " << endl;
	// testMath<vector3<T>, T>();
	// cout << " == Vector4 == " << endl;
	// testMath<vector4<T>, T>();
}




//Main program
// 
int main(int, char **) {

	cout << " ==+++++++++== " << endl;
	cout << " ==  Float  == " << endl;
	cout << " ==+++++++++== " << endl;
	testAllVectorCompile<float>();
	cout << " ==+++++++++== " << endl;
	cout << " == Double  == " << endl;
	cout << " ==+++++++++== " << endl;
	testAllVectorCompile<double>();

	return 0;
}
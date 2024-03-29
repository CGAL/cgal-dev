
/*!
\ingroup PkgKernelDKernelConcept
\cgalConcept

\cgalRefines{AdaptableUnaryFunction}

\sa `Kernel_d::CartesianConstIterator_d`

*/
class Kernel_d::ConstructCartesianConstIterator_d {
public:

/// \name Operations
/// A model of this concept must provide:
/// @{

/*!
returns an iterator on the zeroth %Cartesian coordinate of `p`.
*/
Kernel_d::Cartesian_const_iterator_d operator()(const Kernel_d::Point_d
&p);

/*!
returns the past the end iterator of the %Cartesian coordinates of `p`.
*/
Kernel_d::Cartesian_const_iterator_d operator()(const Kernel_d::Point_d
&p, int);

/// @}

}; /* end Kernel_d::ConstructCartesianConstIterator_d */


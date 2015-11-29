/*
 * QIC_lib (Quantum information and computation library)
 *
 * Copyright (c) 2015 - 2016  Titas Chanda (titas.chanda@gmail.com)
 *
 * This file is part of QIC_lib.
 *
 * QIC_lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QIC_lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QIC_lib.  If not, see <http://www.gnu.org/licenses/>.
 */



namespace qic 
{

  template<typename T1,typename TR =  
	   typename std::enable_if< std::is_arithmetic< pT<T1> >::value, 
				    arma::Mat< eT<T1> > 
				    >::type>
  inline 
  TR Tx(const T1& rho1,
	arma::uvec sys,
	arma::uvec dim)
  {
    auto p = as_Mat(rho1); 
    
    bool checkV = true;
    if(p.n_cols == 1)
      checkV = false;

#ifndef QIC_LIB_NO_DEBUG
    if(p.n_elem == 0)
      throw Exception("qic::Tx",Exception::type::ZERO_SIZE);

    if(checkV)
      if(p.n_rows!=p.n_cols)
	throw Exception("qic::Tx",
			Exception::type::MATRIX_NOT_SQUARE_OR_CVECTOR);
    
    if( dim.n_elem == 0 || arma::any(dim == 0))
      throw Exception("qic::Tx",Exception::type::INVALID_DIMS);
    
    if( arma::prod(dim)!= p.n_rows)
      throw Exception("qic::Tx",Exception::type::DIMS_MISMATCH_MATRIX);

    if(dim.n_elem < sys.n_elem || arma::any(sys == 0) 
       || arma::any(sys > dim.n_elem)  
       || sys.n_elem != arma::find_unique(sys,false).eval().n_elem)
      throw Exception("qic::Tx",Exception::type::INVALID_SUBSYS);
#endif

    sys = arma::sort(sys);  
    _internal::protect_subs::dim_collapse_sys(dim,sys);
    const arma::uword n = dim.n_elem; 
    
    if(!checkV)
      p *= p.t();

    arma::uvec product=arma::ones<arma::uvec>(n);
    for(arma::sword i=n-2;i>=0; --i)
      product.at(i)=product.at(i+1)*dim.at(i+1);
    
    const arma::uword loop_no = 2*n;
    arma::uword* loop_counter = new arma::uword [loop_no+1];
    arma::uword* MAX = new arma::uword [loop_no+1];

    for(arma::uword i=0;i<n;++i)
      {
	MAX[i]=dim.at(i);
	MAX[i+n]=dim.at(i);
      }
    MAX[loop_no]=2; 
   
    for(arma::uword i=0; i<loop_no+1; ++i)
      loop_counter[i]=0;
          
    arma::uword p1=0; 
       
    while(loop_counter[loop_no]==0)
      {

	arma::uword I(0),J(0), K(0), L(0); 
	for(arma::uword i = 0 ; i < n ; ++i)
	  {
	    I += product.at(i)*loop_counter[i];
	    J += product.at(i)*loop_counter[i+n];

	    if( arma::any(sys==i+1) ) 
	      {
		K += product.at(i)*loop_counter[i+n];
		L += product.at(i)*loop_counter[i];
	      } 
	    else
	      {
		K += product.at(i)*loop_counter[i];
		L += product.at(i)*loop_counter[i+n];
	      }
	  }
	
	if(I>K)
	  std::swap(p.at(I,J),p.at(K,L));
	  

	++loop_counter[0];
	while(loop_counter[p1]==MAX[p1])
	  {
	    loop_counter[p1]=0;
	    loop_counter[++p1]++;
	    if(loop_counter[p1]!=MAX[p1])
	      p1=0;
	  }
      }
	
    delete [] loop_counter;
    delete [] MAX;
    return p;
  }





  template<typename T1, typename TR = 
	   typename std::enable_if< std::is_arithmetic< pT<T1> >::value, 
				    arma::Mat< eT<T1> > 
				    >::type>
  inline 
  TR Tx(const T1& rho1,
	arma::uvec sys,
	arma::uword dim = 2)
  {
    const auto& p = as_Mat(rho1); 
    
    bool checkV = true;
    if(p.n_cols == 1)
      checkV = false;

#ifndef QIC_LIB_NO_DEBUG
    if(p.n_elem == 0)
      throw Exception("qic::Tx",Exception::type::ZERO_SIZE);

    if(checkV)
      if(p.n_rows!=p.n_cols)
	throw Exception("qic::Tx",
			Exception::type::MATRIX_NOT_SQUARE_OR_CVECTOR);
    
    if( dim == 0)
      throw Exception("qic::Tx",Exception::type::INVALID_DIMS);
#endif

    arma::uword n = static_cast<arma::uword>
      (std::llround(std::log(p.n_rows) / std::log(dim)));

    arma::uvec dim2 = dim*arma::ones<arma::uvec>(n);
    return Tx(p,std::move(sys),std::move(dim2));

  }



}
#include "../../../include/crystalPlasticity.h"

//implementation of the getElementalValues method
template <int dim>
void crystalPlasticity<dim>::getElementalValues(FEValues<dim>& fe_values,
						unsigned int dofs_per_cell,
						unsigned int num_quad_points,
						FullMatrix<double>& elementalJacobian,
						Vector<double>&     elementalResidual)
 {

     //Initialized history variables and pfunction variables if unititialized
     if(initCalled == false){
	 init(num_quad_points);
     }

     unsigned int cellID = fe_values.get_cell()->user_index();
     std::vector<unsigned int> local_dof_indices(dofs_per_cell);
     Vector<double> Ulocal(dofs_per_cell);

     typename DoFHandler<dim>::active_cell_iterator cell(& this->triangulation,
							 fe_values.get_cell()->level(),
							 fe_values.get_cell()->index(),
							 & this->dofHandler);
     cell->set_user_index(fe_values.get_cell()->user_index());
     cell->get_dof_indices (local_dof_indices);
     for(unsigned int i=0; i<dofs_per_cell; i++){
	 Ulocal[i] = this->solutionWithGhosts[local_dof_indices[i]];
     }

     //local data structures
     FullMatrix<double> K_local(dofs_per_cell,dofs_per_cell),CE_tau(dim,dim),E_tau(dim,dim),temp,temp2,temp3;
     Vector<double> Rlocal (dofs_per_cell);
     K_local = 0.0; Rlocal = 0.0;


     //loop over quadrature points
     for (unsigned int q=0; q<num_quad_points; ++q){
	 //Get deformation gradient
	 F=0.0;
	 for (unsigned int d=0; d<dofs_per_cell; ++d){
	     unsigned int i = fe_values.get_fe().system_to_component_index(d).first;
	     for (unsigned int j=0; j<dim; ++j){
		 F[i][j]+=Ulocal(d)*fe_values.shape_grad(d, q)[j]; // u_{i,j}= U(d)*N(d)_{,j}, where d is the DOF correonding to the i'th dimension
	     }
	 }
	 for (unsigned int i=0; i<dim; ++i){
	     F[i][i]+=1;
	 }

	 //Update strain, stress, and tangent for current time step/quadrature point
	 calculatePlasticity(cellID, q);

     //this->pcout<<P[0][0]<<"\t"<<P[1][1]<<"\t"<<P[2][2]<<"\n";

	 //Fill local residual
	 for (unsigned int d=0; d<dofs_per_cell; ++d) {
	     unsigned int i = fe_values.get_fe().system_to_component_index(d).first;
	     for (unsigned int j = 0; j < dim; j++){
		 Rlocal(d) -=  fe_values.shape_grad(d, q)[j]*P[i][j]*fe_values.JxW(q);
	     }

	 }

	 

         //calculate von-Mises stress and equivalent strain
         double traceE, traceT,vonmises,eqvstrain;
         FullMatrix<double> deve(dim,dim),devt(dim,dim);


         traceE=E_tau.trace();
         traceT=T.trace();
         temp=IdentityMatrix(3);
         temp.equ(traceE/3,temp);

         deve=E_tau;
         deve.add(-1.0,temp);

         temp=IdentityMatrix(3);
         temp.equ(traceT/3,temp);

         devt=T;
         devt.add(-1.0,temp);

         vonmises= devt.frobenius_norm();
         vonmises=sqrt(3.0/2.0)*vonmises;
         eqvstrain=deve.frobenius_norm();
         eqvstrain=sqrt(2.0/3.0)*eqvstrain;

         //fill in post processing field values

				 this->postprocessValues(cellID, q, 0, 0)=vonmises;
         this->postprocessValues(cellID, q, 1, 0)=eqvstrain;
				 if(this->userInputs.enableTwinning)
	         this->postprocessValues(cellID, q, 2, 0)=twin_ouput[cellID][q];

				 //evaluate elemental stiffness matrix, K_{ij} = N_{i,k}*C_{mknl}*F_{im}*F{jn}*N_{j,l} + N_{i,k}*F_{kl}*N_{j,l}*del{ij} dV
				 for (unsigned int d1=0; d1<dofs_per_cell; ++d1) {
			     unsigned int i = fe_values.get_fe().system_to_component_index(d1).first;
			     for (unsigned int d2=0; d2<dofs_per_cell; ++d2) {
						 unsigned int j = fe_values.get_fe().system_to_component_index(d2).first;
						 for (unsigned int k = 0; k < dim; k++){
						   for (unsigned int l= 0; l< dim; l++){
								 K_local(d1,d2) +=  fe_values.shape_grad(d1, q)[k]*dP_dF[i][k][j][l]*fe_values.shape_grad(d2, q)[l]*fe_values.JxW(q);
				     	 }
						 }
			     }
				 }
     }
		 this->postprocessValuesAtCellCenters(cellID,0)=cellOrientationMap[cellID];
     elementalJacobian = K_local;
     elementalResidual = Rlocal;

 }

 #include "../../../include/crystalPlasticity_template_instantiations.h"

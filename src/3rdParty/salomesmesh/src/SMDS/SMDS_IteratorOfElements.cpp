// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  SMESH SMDS : implementation of Salome mesh data structure
//
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "SMDS_IteratorOfElements.hxx"

bool SMDS_IteratorOfElements::subMore()
{
        if((t2Iterator.get()==NULL)||(!t2Iterator->more()))
        {
                if(t1Iterator->more())
                {
                        t2Iterator=t1Iterator->next()->elementsIterator(myType);
                        return subMore();
                }
                else return false;
        }
        else return true;
}

const SMDS_MeshElement * SMDS_IteratorOfElements::subNext()
{
        if((t2Iterator.get()==NULL)||(!t2Iterator->more()))
                if(t1Iterator->more())
                        t2Iterator=t1Iterator->next()->elementsIterator(myType);
        return t2Iterator->next();
}

/////////////////////////////////////////////////////////////////////////////
/// Create an iterator which look for elements of type type which are linked 
/// to the element element. it is the iterator to get connectivity of element
//////////////////////////////////////////////////////////////////////////////
SMDS_IteratorOfElements::SMDS_IteratorOfElements(const SMDS_MeshElement * element,
                                                 SMDSAbs_ElementType type,
                                                 const SMDS_ElemIteratorPtr& it)
     : t1Iterator(it),
       t2Iterator(SMDS_ElemIteratorPtr((SMDS_ElemIterator*)NULL)),
       myType(type), myElement(element),
       myProxyElement(NULL)
{
        while(subMore())
                alreadyReturnedElements.insert(subNext());
        itAlreadyReturned= alreadyReturnedElements.begin();
        switch(myElement->GetType())
        {
        case SMDSAbs_Node: 
        case SMDSAbs_Edge: myReverseIteration=true; break;
        case SMDSAbs_Face: myReverseIteration=(type==SMDSAbs_Volume); break;
        default: myReverseIteration=false;
        }       
}

bool SMDS_IteratorOfElements::more()
{
        if(myProxyElement==NULL)
        {
                while(itAlreadyReturned!=alreadyReturnedElements.end())
                {
                        myProxyElement=*itAlreadyReturned;
                        itAlreadyReturned++;    

                        if(myReverseIteration)
                        {
                                SMDS_ElemIteratorPtr it=
                                        myProxyElement->elementsIterator(myElement->GetType());
                                while(it->more())
                                {                               
                                        if(it->next()==myElement) return true;
                                }
                        }
                        else return true;
                }
                myProxyElement=NULL;
                return false;
        }
        else return true;
}

const SMDS_MeshElement * SMDS_IteratorOfElements::next()
{
        more();
        const SMDS_MeshElement *e=myProxyElement;
        myProxyElement=NULL;
        return e;
}

#include <vector>
#include <string>
#include <deque>
#include <utils/common/UtilExceptions.h>
#include "ROEdge.h"
#include "ROEdgeVector.h"

using namespace std;

ROEdgeVector::ROEdgeVector()
{
}


ROEdgeVector::~ROEdgeVector()
{
}


void 
ROEdgeVector::add(ROEdge *edge)
{
    _edges.push_back(edge);
}


std::ostream &operator<<(std::ostream &os, const ROEdgeVector &ev)
{
    for(ROEdgeVector::EdgeVector::const_iterator j=ev._edges.begin(); j!=ev._edges.end(); j++) {
        if(j!=ev._edges.begin()) {
            os << " ";
        }
        os << (*j)->getID();
    }
    return os;
}


ROEdge *
ROEdgeVector::getFirst() const
{
    if(_edges.size()==0) {
        throw OutOfBoundsException();
    }
    return _edges[0];
}


ROEdge *
ROEdgeVector::getLast() const
{
    if(_edges.size()==0) {
        throw OutOfBoundsException();
    }
    return _edges[_edges.size()-1];
}



std::deque<std::string> 
ROEdgeVector::getIDs() const
{
    std::deque<std::string> ret;
    for(EdgeVector::const_iterator i=_edges.begin(); i!=_edges.end(); i++) {
        ret.push_back((*i)->getID());
    }
    return ret;
}


double 
ROEdgeVector::recomputeCosts(long time) const
{
    double costs = 0;
    for(EdgeVector::const_iterator i=_edges.begin(); i!=_edges.end(); i++) {
        costs += (*i)->getCost(time);
        time += (*i)->getDuration(time);
    }
    return costs;
}


bool
ROEdgeVector::equals(const ROEdgeVector &vc) const
{
    if(size()!=vc.size()) {
        return false;
    }
    for(size_t i=0; i<size(); i++) {
        if(_edges[i]!=vc._edges[i]) {
            return false;
        }
    }
    return true;
}


size_t 
ROEdgeVector::size() const
{
    return _edges.size();
}


ROEdgeVector
ROEdgeVector::getReverse() const 
{
    ROEdgeVector ret;
    for(EdgeVector::const_reverse_iterator i=_edges.rbegin(); i!=_edges.rend(); i++) {
        ret.add(*i);
    }
    return ret;
}

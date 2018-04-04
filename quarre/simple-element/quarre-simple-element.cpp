#include "quarre-simple-element.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>


using namespace score::addons;

quarre::SimpleElement::SimpleElement(const Id<quarre::SimpleElement> &id, QObject *parent) :
    IdentifiedObject(id, "SimpleElement", parent)
{


}

quarre::SimpleElement::~SimpleElement() {}

template <> void DataStreamReader::read(
        const quarre::SimpleElement& e )
{
    insertDelimiter();

}

template <> void DataStreamWriter::write(
        quarre::SimpleElement& e )
{
    checkDelimiter();
}


template <> void JSONObjectReader::read(
        const quarre::SimpleElement& e )
{

}

template <> void JSONObjectWriter::write(
        quarre::SimpleElement& e )
{

}


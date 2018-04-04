#include "quarre-process-model.hpp"
#include <QString>

using namespace score::addons::quarre;

score::addons::quarre::ProcessModel::ProcessModel(
        const TimeVal& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent ) : ProcessModel(duration, id, "quarrè-process", parent)
{
    metadata().setInstanceName(*this);
}

QString score::addons::quarre::ProcessModel::prettyName() const
{
    return tr("quarrè-process");
}

void score::addons::quarre::ProcessModel::startExecution()
{

}

void score::addons::quarre::ProcessModel::stopExecution()
{

}

void score::addons::quarre::ProcessModel::reset()
{

}

ProcessStateDataInterface* score::addons::quarre::ProcessModel::startStateData() const
{
    return nullptr;
}

ProcessStateDataInterface* score::addons::quarre::ProcessModel::endStateData() const
{
    return nullptr;
}

Selection score::addons::quarre::ProcessModel::selectableChildren() const
{
    return {};
}

Selection score::addons::quarre::ProcessModel::selectedChildren() const
{
    return {};
}

void score::addons::quarre::ProcessModel::setSelection(const Selection &s) const
{

}

void score::addons::quarre::ProcessModel::setDurationAndScale(const TimeVal &newDuration)
{

}

void score::addons::quarre::ProcessModel::setDurationAndGrow(const TimeVal &newDuration)
{

}

void score::addons::quarre::ProcessModel::setDurationAndShrink(const TimeVal &newDuration)
{

}

// SERIALIZATION --------------------------------------------------------------------------

template <> void DataStreamReader::read(
        const score::addons::quarre::ProcessModel& process )
{
    m_stream << (int32_t) process.simple_elements.size();
    for ( const auto& e : process.simple_elements )
        readFrom(e);

    m_stream << (int32_t) process.polymorphic_entities.size();
    for ( const auto& e : process.polymorphic_entities )
        readFrom(e);

    insertDelimiter();
}

template <> void DataStreamWriter::write(
        score::addons::quarre::ProcessModel& process )
{
    int32_t     simple_count;
    m_stream >> simple_count;

    for ( ; simple_count-- > 0; )
        process.simple_elements.add(
                    new score::addons::quarre::SimpleElement(*this, &process) );

    int32_t     poly_count;
    m_stream >> poly_count;

    auto& pl = components.interfaces<score::addons::quarre::PolyMorphicElementFactoryList>();

    for ( ; poly_count-- > 0; )
    {
        auto e = deserialize_interface(pl, *this, &process);
        if ( e ) process.polymorphic_entities.add ( e );
        else throw std::runtime_error("Unable to load.");
    }

    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const score::addons::quarre::ProcessModel& process )
{
    obj [ "SimpleElements" ] = toJsonArray(process.simple_elements);
    obj [ "PolyElements"   ] = toJsonArray(process.polymorphic_entities);
}

template <> void JSONObjectWriter::write(
        score::addons::quarre::ProcessModel& process )
{
    for ( const auto& json_vref : obj [ "SimpleElements"].toArray())
    {
        JSONObject::Deserializer deserializer(json_vref.toObject());
        process.simple_elements.add ( new score::addons::quarre::SimpleElement(deserializer, &process));
    }

    auto& pl = components.interfaces<score::addons::quarre::PolymorphicElementFactoryList>();
    for ( const auto& json_vref : obj [ "PolyElements"].toArray())
    {
        JSONObject::Deserializer deserializer(json_vref.toObject());
        auto e = deserialize_interface(pl, deserializer, &process);
        if ( e ) process.polymorphic_entities.add ( e );
        else throw std::runtime_error("Unable to load.");
    }
}




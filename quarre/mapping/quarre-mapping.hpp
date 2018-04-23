#pragma once
#include <ossia/ossia.hpp>
#include <QString>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>


#include <score/model/IdentifiedObject.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class mapping final : public IdentifiedObject<score::addons::quarre::mapping>
{
    Q_OBJECT
    SCORE_SERIALIZE_FRIENDS

    Q_PROPERTY  ( QString source READ source WRITE on_source_changed )
    Q_PROPERTY  ( QString destination READ destination WRITE on_destination_changed )
    Q_PROPERTY  ( QString expression READ expression WRITE on_expression_changed)

    public: // ----------------------------------------------------
    mapping ( const Id<mapping>& id, QObject* parent );

    template <typename impl> mapping(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    QVBoxLayout* layout             () const;
    const QString source            () const;
    const QString destination       () const;
    const QString expression        () const;
    const QString expression_js     () const;

    public slots:
    void on_source_changed         ( const QString& source );
    void on_destination_changed    ( const QString& destination );
    void on_expression_changed     ( const QString& expression );

    private: // ---------------------------------------------------
    QString m_source;
    QString m_destination;
    QString m_expression;
};

}
}
}

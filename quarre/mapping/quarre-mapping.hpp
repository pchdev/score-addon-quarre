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

    Q_PROPERTY  ( QString source READ source WRITE set_source NOTIFY sourceChanged )
    Q_PROPERTY  ( QString destination READ destination WRITE set_destination NOTIFY destinationChanged )
    Q_PROPERTY  ( QString expression READ expression WRITE set_expression NOTIFY expressionChanged )

    public: // ----------------------------------------------------
    mapping ( const Id<mapping>& id, QObject* parent );

    template <typename impl> mapping(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    QVBoxLayout* layout        () const;
    const QString source       () const;
    const QString destination  () const;
    const QString expression   () const;

    void set_source         ( const QString& source );
    void set_destination    ( const QString& destination );
    void set_expression     ( const QString& expression );

    signals: // ---------------------------------------------------
    void sourceChanged          ();
    void destinationChanged     ();
    void expressionChanged      ();
    void minusButtonPressed     ();

    private: // ---------------------------------------------------
    QLineEdit* m_source;
    QLineEdit* m_destination;
    QTextEdit* m_expression;
    QPushButton* m_minus_button;
    QVBoxLayout* m_layout;
};

}
}
}
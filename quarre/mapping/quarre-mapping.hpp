#pragma once
#include <ossia/ossia.hpp>
#include <QString>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <quarre/interaction/quarre-interaction.hpp>

#include <score/model/IdentifiedObject.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class Mapping final : public IdentifiedObject<Mapping>
{
    Q_OBJECT
    SCORE_SERIALIZE_FRIENDS

    Q_PROPERTY  ( QString source READ source WRITE set_source NOTIFY sourceChanged )
    Q_PROPERTY  ( QString destination READ destination WRITE set_destination NOTIFY destinationChanged )
    Q_PROPERTY  ( QString expression READ expression WRITE set_expression NOTIFY expressionChanged )

    public: // ----------------------------------------------------
    Mapping ();
    Mapping ( const Id<Mapping>& id, QObject* parent );

    template <typename impl> Mapping(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    QVBoxLayout* layout         () const;
    const QString& source       () const;
    const QString& destination  () const;
    const QString& expression   () const;

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

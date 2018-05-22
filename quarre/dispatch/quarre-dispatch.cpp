#include "quarre-dispatch.hpp"

using namespace score::addons;

bool quarre::dispatcher::dispatch_incoming_interaction(quarre::interaction &i)
{
    // candidate algorithm
    // eliminate non-connected clients
    // eliminate clients that cannot support the requested inputs
    // eliminate clients that already have an incoming interaction
    auto& srv = quarre::server::instance();

    if ( i.dispatch_all() )
    {
        auto p = srv.get_common_parameter("/interactions/next/incoming");
        p->push_value(i.to_list());
        return true;
    }

    std::vector<dispatcher::candidate> candidates;

    for ( const auto& user : srv.m_users )
    {
        dispatcher::candidate candidate;

        candidate.target    = user;
        candidate.priority  = 0;

        switch ( user->m_status )
        {
        case user_status::DISCONNECTED:        goto next;
        case user_status::INCOMING:            goto next;
        case user_status::INCOMING_ACTIVE:     goto next;

        case user_status::IDLE:
            goto check_module; // priority stays at 0

        case user_status::ACTIVE:
        {
            auto acd = user->get_active_countdown();
            if ( i.countdown() < acd+5) goto next;

            candidate.priority = 1;
            goto check_module;
        }
        }

        check_module:
        if ( !user->supports_interaction(i))
            goto next;

        select:
        candidate.priority += user->m_interaction_count;
        candidates.push_back(candidate);

        next:
        continue;
    }

    // the candidate with the lowest priority will be selected
    // if there is no candidate, interaction will not be dispatched
    if ( candidates.size() == 0 ) return false;

    dispatcher::candidate* winner = 0;

    for ( auto& candidate : candidates )
    {
        if ( ! winner )
        {
            winner = &candidate;
            continue;
        }
        // if its a draw between two or more candidates,
        // select randomly between them
        if ( candidate.priority == winner->priority )
        {
            std::srand ( std::time(nullptr) );
            int r = std::rand();
            r %= 2;

            if ( r ) winner = &candidate;
        }

        else if ( candidate.priority < winner->priority )
            winner = &candidate;
    }

    if ( winner->target )
    {
        winner->target->set_incoming_interaction(i);
        return true;
    }

    else return false;
}

void quarre::dispatcher::dispatch_active_interaction(
        quarre::interaction& i, const Device::DeviceList& devlist )
{
    auto& srv = quarre::server::instance();
    if ( i.dispatch_all() )
    {
        auto p = srv.get_common_parameter("/interactions/next/begin");
        p->push_value(i.to_list());
    }

    else for ( const auto& user : srv.m_users )
        if ( user->m_incoming_interaction == &i )
            user->set_active_interaction(i, devlist);
}

void quarre::dispatcher::dispatch_ending_interaction(quarre::interaction &i)
{
    auto& srv = quarre::server::instance();
    if ( i.dispatch_all() )
    {
        auto p = srv.get_common_parameter("/interactions/current/end");
        p->push_value(i.to_list());
    }

    else for ( const auto& user : srv.m_users )
        if ( user->m_active_interaction == &i )
            user->end_interaction(i);
}

void quarre::dispatcher::dispatch_paused_interaction(quarre::interaction &i)
{

}

void quarre::dispatcher::dispatch_resumed_interaction(interaction &i)
{

}

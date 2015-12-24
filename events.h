#ifndef JBWM_EVENTS_H
#define JBWM_EVENTS_H

#ifdef DEBUG
void
print_atom(const Atom a, const unsigned int line);
#endif//DEBUG

void main_event_loop(void);

void jbwm_current_to_head(void);

#endif /* ! JBWM_EVENTS_H */

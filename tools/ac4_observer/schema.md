# Journal schema v1

The journal is UTF-8 JSON Lines. Every line is an independent object. `session_start` contains the session ID, UTC start time, QPC frequency, and source modes. `event` lines include `seq`, `qpc`, UTC time, thread ID, source, event type, control/logical slot ID, raw state, process-local object/recipient pointer values, internal discriminator, and correlation fields. `session_end` includes completion and dropped-event counters.

## Sources

| Source | Event | Meaning |
| --- | --- | --- |
| `xinput_poll` | `button_edge` | XInput button or trigger-threshold press/release; `state` is 1 for press and 0 for release. |
| `xinput_poll` | `analog_value` | Raw XInput trigger byte or signed stick-axis sample encoded as an unsigned 16-bit value. |
| `xinput_poll` | `controller_connection` | Controller slot connected/disconnected. |
| `logical_edge_query` | `rising_edge_result` | Result and logical slot passed to the statically identified rising-edge query. |
| `logical_gate` | `context_block_result` | Return of the context-conflict check in the callback path. |
| `logical_hold_query` | `hold_or_repeat_result` | Return of a time/state query; exact user-facing meaning remains unverified. |
| `logical_availability` | `availability_result` | Return of the combined availability/consumption check. |
| `native_callback` | `callback_enter`, `callback_return` | Entry/return around the statically identified callback, including its session-local object pointer. |
| `native_resolver` | `resolver_call` | Resolver call evidence and raw output-pointer value. The pointed-to structure is not read. |
| `native_dispatch` | `candidate_dispatch` | Flags, handler pointer, and unlabeled internal discriminator returned by the conditional virtual dispatcher. This is not proof of action start. |
| `status` | `native_hook_status` | 1 means all exact-profile hooks installed; 0 means the native source was disabled. |

`correlation_id` connects nested internal observations on the same callback stack. `physical_event_id` identifies a sampled physical event. `physical_ref` points to a recent XInput button/trigger press. When `correlation_method` contains `temporal`, it expresses only a 250 ms time-nearness inference; it does not prove causality or that the game's DirectInput proxy saw the same input.

Objects are represented as hexadecimal pointer values and are valid only within one process. Unknown semantics remain numeric and must not be relabeled based on button names, animation names, or a single temporal match.

## Recovery and merging

Pass multiple journal paths to `analyze.py` to merge sessions. Records are sorted by monotonic QPC within a session. A truncated final line is ignored and reported; malformed non-final lines fail loudly. A session without `session_end` is marked incomplete. Analyzer evidence references use `filename:line` and do not include absolute user paths.

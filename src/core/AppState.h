#pragma once

/**
 * FocusCycle Application States
 */
enum class AppState {
    WorkWait,      ///< Initial / Post-break state (WorkStartWindow shown)
    Working,       ///< Work timer counting down & negative overtime counting
    BreakWait,     ///< Idle threshold reached (BreakDialog shown)
    Break,         ///< Break timer counting down
    CompanyBreak   ///< Scheduled company break dialog shown
};

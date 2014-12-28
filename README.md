# illumos-core

illumos core README - Aug 14, 2014.

Copyright 2014 Garrett D'Amore <garrett@damore.org>

This tree (branch really) represents a fork of illumos-gate.  This is
for my personal use / playground.  It has substantially different integration
rules than illumos-gate proper.

illumos-core may one day become a shared work area, but if so, it will
follow the guidelines set forth here.  Unlike illumos-gate proper, there
is a BDFL, and his name is Garrett D'Amore (that's me).

GOALS
=====

These are our goals.  We don't achieve all these yet, but this is the vision,
or mission statement.  These goals are not listed in any particular order.

 * POSIX compliant, by default.  Even at the expense of compatibility
   with legacy.

 * The system should be able to self-host.  By that, it should be possible
   to build a distribution that is complete enough to build the system itself.

 * Minimal dependencies.  Eliminating dependencies on things like perl, Java,
   etc. is a supporting goal of the above mission.

 * Portable.  The system should support more than just little-endian x86.
   We'd like to support SPARC, and someday ARM64.

 * Leaner.  Things that add no value, don't support the POSIX mission, or
   typically not part of the core system (desktop components, web servers,
   etc.) belong elsewhere.  Exceptions for components that add unique value,
   such as iSCSI target, etc. are appropriate.  This maps well to the
   illumos-gate goals.

 * Nuke the legacy.  Compatibility with hardware that hasn't been produced in
   decades, or that nobody uses now, or will ever use with illumos, is a
   non-goal.  That stuff is bitrotting... remove it.

 * Stay current against -gate.  The upstream (-gate) has lots of folks doing
   good work.  It is merged here frequently.

 * Compiler modernization and freedom.  We want to support clang/llvm.  These
   are BSD licensed, portable, performant tools.

 * Cross-compilation.  Support for other platforms (SPARC, ARM64) would be a
   lot easier if it were possible to cross-compile the system.

 * More complete testing.  There are tests already, but most of the system does
   not have automatic testing.  We can do better.

 * Support illumos-gate upstream development.  We want to believe that the
   work done here will be useful to others in the upstream.  Achieving this
   means not gratuitiously breaking things illumos-gate needs unless it serves
   other purposes above.  It should be possible to cherry pick changes from
   this to bring back into the upstream.


BIG RULES
=========

 * First and foremost, BDFL has final authoritative say about contents
   of this branch.  Full stop.  No design by committee, no consensus
   decision making.  If you don't like this, go back to -gate.

 * A goal is that each commit should be suitable for inclusion in -gate,
   assuming that someone wanted to do that.  That means always release
   ready-quality, well-formed commits, cstyle and lint clean, etc.

 * Since this is currently just a team of one, there is no peer review and
   No RTI process.  Just send me a pull request.

 * However, every changeset integrated needs to be identified with a bug ID
   from this repository's bug tracker (bitbucket.)

 * Commit comments look like this:

    fixes #<id> <synopsis>
    [optionally illumos formatted commit messages can follow]

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/gdamore/illumos-core?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

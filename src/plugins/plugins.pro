######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += filter
SUBDIRS += verifier
SUBDIRS += znc
qtHaveModule(x11extras):SUBDIRS += gnome

#include <stdint.h>
#include <stdio.h>

#include <Python.h>

#include "hiddata.h"

/* taken from blink1 / hardware / firmware / usbconfig.h */
#define IDENT_VENDOR_NUM        0x27B8
#define IDENT_PRODUCT_NUM       0x01ED
#define IDENT_VENDOR_STRING     "ThingM"
#define IDENT_PRODUCT_STRING    "blink(1)"

static void blink1_raise_error (int);

static PyObject *Blink1Error;

typedef struct
{
  PyObject_HEAD

  usbDevice_t *dev;
} Blink;

static void Blink_dealloc (Blink *);
static int Blink_init (Blink *, PyObject *, PyObject *);
static PyObject *Blink_set_rgb (Blink *, PyObject *);
static PyObject *Blink_fade_to_rgb (Blink *, PyObject *);

static void
Blink_dealloc (Blink * self)
{
  Py_BEGIN_ALLOW_THREADS;
  usbhidCloseDevice (self->dev);
  Py_END_ALLOW_THREADS;

  self->ob_type->tp_free ((PyObject *) self);
}

static int
Blink_init (Blink * self, PyObject * args, PyObject * kwargs)
{
  int rc;
  Py_BEGIN_ALLOW_THREADS;
  rc = usbhidOpenDevice (&self->dev,
                         IDENT_VENDOR_NUM,
                         IDENT_VENDOR_STRING,
                         IDENT_PRODUCT_NUM,
                         IDENT_PRODUCT_STRING,
                         1);	/* NOTE: `0' means "not using report IDs" */
  Py_END_ALLOW_THREADS;

  if (rc == 0)
    return 0;

  blink1_raise_error (rc);
  return -1;
}

static PyObject *
Blink_set_rgb (Blink * self, PyObject * args)
{
  int r, g, b;

  if (!PyArg_ParseTuple (args, "iii", &r, &g, &b))
    return NULL;

  if (r > UINT8_MAX || g > UINT8_MAX || b > UINT8_MAX)
    {
      PyErr_SetString (PyExc_ValueError, "RGB must be in [0-255]");
      return NULL;
    }

  char buf[9];
  buf[0] = 0;
  buf[1] = 'n';
  buf[2] = (char) r;
  buf[3] = (char) g;
  buf[4] = (char) b;

  int rc;
  Py_BEGIN_ALLOW_THREADS;
  rc = usbhidSetReport (self->dev, buf, sizeof (buf));
  Py_END_ALLOW_THREADS;

  if (rc != 0)
    blink1_raise_error (rc);

  Py_RETURN_NONE;
}

static PyObject *
Blink_fade_to_rgb (Blink * self, PyObject * args)
{
  int fade_ms;
  int r, g, b;

  if (!PyArg_ParseTuple (args, "iiii", &fade_ms, &r, &g, &b))
    return NULL;

  if (r > UINT8_MAX || g > UINT8_MAX || b > UINT8_MAX)
    {
      PyErr_SetString (PyExc_ValueError, "RGB must be in [0-255]");
      return NULL;
    }

  int dms = fade_ms / 10;	/* millis_divided_by_10 */

  char buf[9];

  buf[0] = 0;
  buf[1] = 'c';
  buf[2] = (char) r;
  buf[3] = (char) g;
  buf[4] = (char) b;
  buf[5] = (dms >> 8);
  buf[6] = dms % 0xff;

  int rc;
  Py_BEGIN_ALLOW_THREADS;
  rc = usbhidSetReport (self->dev, buf, sizeof (buf));
  Py_END_ALLOW_THREADS;

  if (rc != 0)
    blink1_raise_error (rc);

  Py_RETURN_NONE;
}

static void
blink1_raise_error (int rc)
{
  static char buf[80];

  switch (rc)
    {
    case USBOPEN_ERR_ACCESS:
      PyErr_SetString (Blink1Error, "Access to device denied");
      break;
    case USBOPEN_ERR_NOTFOUND:
      PyErr_SetString (Blink1Error, "The specified device was not found");
      break;
    case USBOPEN_ERR_IO:
      PyErr_SetString (Blink1Error, "Communication error with device");
      break;
    default:
      sprintf (buf, "Unknown USB error %d", rc);
      buf[sizeof (buf) - 1] = '\0';
      PyErr_SetString (Blink1Error, buf);
    }
}

static PyMethodDef Blink_methods[] = {
  {"set_rgb", (PyCFunction) Blink_set_rgb, METH_VARARGS, NULL},
  {"fade_to_rgb", (PyCFunction) Blink_fade_to_rgb, METH_VARARGS, NULL},
  {NULL, NULL, 0, NULL}		/* Sentinel */
};

static PyTypeObject BlinkType = {
  PyObject_HEAD_INIT (NULL) 0,	/* ob_size */
  "blink1.Blink",		/* tp_name */
  sizeof (Blink),		/* tp_basicsize */
  0,				/* tp_itemsize */
  (destructor) Blink_dealloc,	/* tp_dealloc */
  0,				/* tp_print */
  0,				/* tp_getattr */
  0,				/* tp_setattr */
  0,				/* tp_compare */
  0,				/* tp_repr */
  0,				/* tp_as_number */
  0,				/* tp_as_sequence */
  0,				/* tp_as_mapping */
  0,				/* tp_hash */
  0,				/* tp_call */
  0,				/* tp_str */
  0,				/* tp_getattro */
  0,				/* tp_setattro */
  0,				/* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* tp_flags */
  "Blink objects",		/* tp_doc */
  0,				/* tp_traverse */
  0,				/* tp_clear */
  0,				/* tp_richcompare */
  0,				/* tp_weaklistoffset */
  0,				/* tp_iter */
  0,				/* tp_iternext */
  Blink_methods,		/* tp_methods */
  0,				/* tp_members */
  0,				/* tp_getset */
  0,				/* tp_base */
  0,				/* tp_dict */
  0,				/* tp_descr_get */
  0,				/* tp_descr_set */
  0,				/* tp_dictoffset */
  (initproc) Blink_init,	/* tp_init */
  0,				/* tp_alloc */
  PyType_GenericNew,		/* tp_new */
};

static PyMethodDef blink1_methods[] = {
  {NULL, NULL, 0, NULL}		/* Sentinel */
};

PyMODINIT_FUNC
initblink1 (void)
{
  PyObject *m;

  if (PyType_Ready (&BlinkType) < 0)
    return;

  m = Py_InitModule ("blink1", blink1_methods);
  if (m == NULL)
    return;

  Py_INCREF (&BlinkType);
  PyModule_AddObject (m, "Blink", (PyObject *) & BlinkType);

  Blink1Error = PyErr_NewException ("blink1.Error", NULL, NULL);
  Py_INCREF (Blink1Error);
  PyModule_AddObject (m, "Error", Blink1Error);
}

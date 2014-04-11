/*
 * python_lib-repa.c
 *
 *  Created on: 12/07/2012
 *      Author: Héberte Fernandes de Moraes
 */

#include <Python.h>

#include <stdint.h>

#include <hdr/repa.h>
#include <hdr/util.h>
#include <hdr/linkedlist.h>

static PyObject *RepaError;

static char repaOpen_docs[] = "repaOpen(): Open a communication channel with Repad";
static PyObject* repaOpen(PyObject* self) {
	repa_sock_t sock = repa_open();
	return Py_BuildValue("(ii)", sock.sock_send, sock.sock_recv);
}

static char repaClose_docs[] = "repaClose(sock): Close communication channel with Repad";
static PyObject* repaClose(PyObject* self, PyObject *args) {
	repa_sock_t sock;

	if (PyArg_ParseTuple(args, "(ii)", &sock.sock_send, &sock.sock_recv)) {
		repa_close(sock);
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

	Py_RETURN_NONE;
}

static char prefixToString_docs[] = "prefixToString(prefix): Convert the (Integer) prefix to "
		"string like [X#X#X#X#X#X]";
static PyObject* prefixToString(PyObject* self, PyObject *args) {
	char prefix_str[50];
	uint32_t prefix;

	if (PyArg_ParseTuple(args, "I", &prefix)) {
		repa_print_prefix(prefix, prefix_str);
	} else {
		sprintf(prefix_str, "[error]");
	}

	return Py_BuildValue("s", prefix_str);
}

static char getRepaNodeAddress_docs[] = "getRepaNodeAddress(): Get the node (Integer) prefix\n";
static PyObject* getRepaNodeAddress(PyObject* self, PyObject *args) {
	return Py_BuildValue("I", repa_get_node_address());
}

static char repaSend_docs[] = "repaSend(sock, 'interest', 'data', data_size, prefix_destination): "
		"Send data with interest defined by user to a specific destination (or to group when "
		"prefix is zero)\n";
static PyObject* repaSend(PyObject* self, PyObject* args) {
	repa_sock_t sock;
	prefix_addr_t prefix = 0;
	Py_ssize_t buffer_size, result;
	const char *interest, *buffer;

    /* Wait for two strings and two integers */
    /* Python call is: f(sock, 'interest', 'data', data_size, prefix) */
	if (PyArg_ParseTuple(args, "(ii)ss#|i", &sock.sock_send, &sock.sock_recv, &interest, &buffer, &buffer_size, &prefix)) {
		result = repa_send(sock, interest, buffer, buffer_size, prefix);
	} else {
        PyErr_SetString(RepaError, "Error on read parameters");
        return NULL;
	}

	return Py_BuildValue("i", result);
}

static char repaSendHidden_docs[] = "repaSendHidden(sock, 'interest', 'data', data_size, prefix_destination): "
		"Send data with interest defined by user to a specific destination (or to group when "
		"prefix is zero). But in this case the interest is not showed on other nodes\n";
static PyObject* repaSendHidden(PyObject* self, PyObject* args) {
	repa_sock_t sock;
	prefix_addr_t prefix = 0;
	Py_ssize_t buffer_size, result;
	const char *interest, *buffer;

	/* Wait for two strings and two integers */
	/* Python call is: f('interest', 'data', prefix) */
	if (PyArg_ParseTuple(args, "(ii)ss#|i", &sock.sock_send, &sock.sock_recv, &interest, &buffer, &buffer_size, &prefix)) {
		result = repa_send_hidden(sock, interest, buffer, buffer_size, prefix);
    } else {
    	PyErr_SetString(RepaError, "Error on read parameters");
    	return NULL;
    }

	return Py_BuildValue("i", result);
}

static char registerInterest_docs[] = "registerInterest(sock, 'interest'): Register a interest defined by "
		"application !!\n";
static PyObject* registerInterest(PyObject* self, PyObject* args) {
	repa_sock_t sock;
	int result = 0;
	const char *interest;

	if (PyArg_ParseTuple(args, "(ii)s", &sock.sock_send, &sock.sock_recv, &interest)) {
		result = repa_register_interest(sock, interest);
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

	return Py_BuildValue("i", result);
}

static char unregisterInterest_docs[] = "unregisterInterest(sock, 'interest'): Unregister a interest defined "
		"by application!!\n";
static PyObject* unregisterInterest(PyObject* self, PyObject* args) {
	repa_sock_t sock;
	int result = 0;
	const char *interest;

	if (PyArg_ParseTuple(args, "(ii)s", &sock.sock_send, &sock.sock_recv, &interest)) {
		result = repa_unregister_interest(sock, interest);
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

	return Py_BuildValue("i", result);
}

static char unregisterAll_docs[] = "unregisterAll(sock): Unregister all interest already registered "
		"by application!!\n";
static PyObject* unregisterAll(PyObject* self, PyObject* args) {
	repa_sock_t sock;
	int result = 0;

	if (PyArg_ParseTuple(args, "(ii)", &sock.sock_send, &sock.sock_recv)) {
		result = repa_unregister_all(sock);
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

    return Py_BuildValue("i", result);
}

static char getInterestsInNetwork_docs[] = "getInterestsInNetwork(sock): Get a list of the latest interest "
		"seen on the network\n";
static PyObject* getInterestsInNetwork(PyObject* self, PyObject* args) {
	PyObject *pylist = NULL;
	repa_sock_t sock;
	int i;
	struct dllist *list = NULL;
	struct dll_node *aux = NULL;

	if (PyArg_ParseTuple(args, "(ii)", &sock.sock_send, &sock.sock_recv)) {
		dll_create(list); // Create a linkedlist

		// Get the list of interest in network in a linkedlist and parse to python's list
		if (repa_get_interest_in_network(sock, list) >= 0) {
			// Create a list in python with the same size of linkedlist
			pylist = PyTuple_New(list->num_elements);

			// Put all the objects in the linkedlist into a python's list
			for (aux = list->head, i = 0; aux != NULL; aux = aux->next, i++) {
				PyTuple_SetItem(pylist, i, PyString_FromString((char*)aux->data));
			}
		}

		dll_destroy_all(list); // Destroy the linkedlist
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

    return pylist;
}

static char getInterestsRegistered_docs[] = "getInterestsRegistered(sock): Get a list of interest "
		"registered\n";
static PyObject* getInterestsRegistered(PyObject* self, PyObject* args) {
	PyObject *pylist = NULL;
	repa_sock_t sock;
	int i;
	struct dllist *list = NULL;
	struct dll_node *aux = NULL;

	if (PyArg_ParseTuple(args, "(ii)", &sock.sock_send, &sock.sock_recv)) {
		dll_create(list); // Create a linkedlist

		// Get the list of interest in network in a linkedlist and parse to python's list
		if (repa_get_interests_registered(sock, list) >= 0) {
			// Create a list in python with the same size of linkedlist
			pylist = PyTuple_New(list->num_elements);

			// Put all the objects in the linkedlist into a python's list
			for (aux = list->head, i = 0; aux != NULL; aux = aux->next, i++) {
				PyTuple_SetItem(pylist, i, PyString_FromString((char*)aux->data));
			}
		}

		dll_destroy(list); // Destroy the linkedlist
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

    return pylist;
}

static char getNodesInNetwork_docs[] = "getNodesInNetwork(sock): Get a list of the latest nodes' prefix "
		"seen on the network\n";
static PyObject* getNodesInNetwork(PyObject* self, PyObject* args) {
	PyObject *pylist = NULL;
	repa_sock_t sock;
	int i;
	struct dllist *list = NULL;
	struct dll_node *aux = NULL;

	if (PyArg_ParseTuple(args, "(ii)", &sock.sock_send, &sock.sock_recv)) {
		dll_create(list); // Create a linkedlist

		// Get the list of nodes in network in a linkedlist and parse to python's list
		if (repa_get_nodes_in_network(sock, list) >= 0) {
			// Create a list in python with the same size of linkedlist
			pylist = PyTuple_New(list->num_elements);

			// Put all the objects in the linkedlist into a python's list
			for (aux = list->head, i = 0; aux != NULL; aux = aux->next, i++) {
				PyTuple_SetItem(pylist, i, PyInt_FromLong((int)*((prefix_addr_t*)aux->data)));
			}
		}

		dll_destroy_all(list); // Destroy the linkedlist
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

    return pylist;
}

static char repaRecv_docs[] = "repaRecv(sock, microseconds): Receive messages with interest already registered. "
		"Exist two ways for call this function: repaRecv(void) wait for a message forever; and "
		"repaRecv(microseconds): waiting for a message for a defined period in microseconds\n";
static PyObject* repaRecv(PyObject* self, PyObject* args) {
	repa_sock_t sock;
	char interest[255];
	char buffer[BUFFER_LEN];
	long int microseconds = 0;
	ssize_t buffer_size = 0;
	prefix_addr_t prefix = 0;

	if (PyArg_ParseTuple(args, "(ii)l", &sock.sock_send, &sock.sock_recv, &microseconds)) {
		if (microseconds == 0)
			microseconds = 1;
		buffer_size = repa_timed_recv(sock, interest, buffer, prefix, microseconds);
	} else {
		PyErr_SetString(RepaError, "Error on read parameters");
		return NULL;
	}

	if (buffer_size > 0) {
		return Py_BuildValue("(ss#i)", interest, buffer, buffer_size, prefix);
	}

	Py_RETURN_NONE;
}

static PyMethodDef repa_methods[] = {
		{"repaOpen", (PyCFunction)repaOpen, METH_VARARGS, repaOpen_docs},
		{"repaClose", (PyCFunction)repaClose, METH_VARARGS, repaClose_docs},
		{"prefixToString", (PyCFunction)prefixToString, METH_VARARGS, prefixToString_docs},
		{"getRepaNodeAddress", (PyCFunction)getRepaNodeAddress, METH_VARARGS, getRepaNodeAddress_docs},
		{"repaSend", (PyCFunction)repaSend, METH_VARARGS, repaSend_docs},
		{"repaSendHidden", (PyCFunction)repaSendHidden, METH_VARARGS, repaSendHidden_docs},
		{"registerInterest", (PyCFunction)registerInterest, METH_VARARGS, registerInterest_docs},
		{"unregisterInterest", (PyCFunction)unregisterInterest, METH_VARARGS, unregisterInterest_docs},
		{"unregisterAll", (PyCFunction)unregisterAll, METH_VARARGS, unregisterAll_docs},
		{"getInterestsInNetwork", (PyCFunction)getInterestsInNetwork, METH_VARARGS, getInterestsInNetwork_docs},
		{"getInterestsRegistered", (PyCFunction)getInterestsRegistered, METH_VARARGS, getInterestsRegistered_docs},
		{"getListNodes", (PyCFunction)getNodesInNetwork, METH_VARARGS, getNodesInNetwork_docs},
		{"repaRecv", (PyCFunction)repaRecv, METH_VARARGS, repaRecv_docs},
		{NULL}
};

PyMODINIT_FUNC initrepa(void) {
	PyObject *module;

	module = Py_InitModule3("repa", repa_methods, "Repa module extension!");

	if (module != NULL) {
	    RepaError = PyErr_NewException("repa.error", NULL, NULL);
	    Py_INCREF(RepaError);
	    PyModule_AddObject(module, "error", RepaError);
	}
}

int main(int argc, char *argv[]) {
    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(argv[0]);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Add a static module */
    initrepa();

    return 0;
}

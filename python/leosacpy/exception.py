from leosacpy.ws import LeosacMessage


class LeosacException(BaseException):
    def __init__(self, msg):
        super().__init__(msg)


class InvalidMessageException(LeosacException):
    def __init__(self, payload):
        msg = 'Invalid websocket message: {}'.format(payload)
        self.payload = payload
        super().__init__(msg)


class APIError(LeosacException):
    """
    A leosac API error 
    """

    def __init__(self, msg: LeosacMessage):
        str_msg = 'WS API Error. Code: {}. Message: {}'.format(msg.status_code,
                                                               msg.status_string)
        self.message = msg
        super().__init__(str_msg)

    def status_code(self):
        return self.message.status_code

    def status_string(self):
        return self.message.status_string


class APIModelException(APIError):
    """
    A Leosac API Error that came from a MODEL_EXCEPTION error code
    """

    def __init__(self, msg: LeosacMessage):
        super().__init__(msg)

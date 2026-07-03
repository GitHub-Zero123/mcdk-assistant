import mcdk_assistant as mcdk


@mcdk.tool("hello", "返回一个问候")
def hello(args, ctx):
    print("hello plugin called")
    return "hello"

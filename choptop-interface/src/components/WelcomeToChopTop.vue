<template>
  <div class="verticalCenter welcome">
    <div>
      <h1>Hello, I'm Choptop!</h1>
      <h2>Press my lower edge to get started</h2>
      <icon name="angle-down" scale="4"></icon>
    </div>
  </div>

</template>

<script>
  export default {
    name: 'WelcomeToChopTop',
    data() {
      return {}
    }
  }
</script>


<style scoped>

  h2 {
    margin-top: 30px;
  }

  .welcome {
    padding-top: 60px;
    width: 760px;
  }

</style>

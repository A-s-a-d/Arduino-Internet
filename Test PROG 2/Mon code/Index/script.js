document.addEventListener('DOMContentLoaded', function() {
    const loginButton = document.querySelector('.btnlogin-popup');
    const modal = document.getElementById('loginModal');
    const closeButton = document.querySelector('.close');

    loginButton.addEventListener('click', () => {
        modal.style.display = 'block';
    });

    closeButton.addEventListener('click', () => {
        modal.style.display = 'none';
    });

    window.addEventListener('click', (event) => {
        if (event.target === modal) {
            modal.style.display = 'none';
        }
    });
});
#ifndef RECTANGLE_3GP_HPP
#define RECTANGLE_3GP_HPP


// STL headers.
#include <algorithm>
#include <utility>


/// <summary>
/// A basic Rectangle class, useful for representing spaces such as screen space.
/// </summary>
template <typename T> class Rectangle final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        /// <summary> Construct a rectangle with a value for each co-ordinate. </summary>
        Rectangle (const T left, const T top, const T right, const T bottom);

        Rectangle()                                     = default;
        Rectangle (const Rectangle& copy)               = default;
        Rectangle& operator= (const Rectangle& copy)    = default;
        ~Rectangle()                                    = default;

        Rectangle (Rectangle&& move);
        Rectangle& operator= (Rectangle&& move);


        ///////////////
        // Operators //
        ///////////////

        /// <summary> Returns a casted version of the rectangle with the desired type. </summary>
        template <typename U> operator Rectangle<U>() const { return { (U) m_left, (U) m_top, (U) m_right, (U) m_bottom }; }

        bool operator== (const Rectangle& rhs) const;
        bool operator!= (const Rectangle& rhs) const;


        /////////////////////////
        // Getters and setters //
        /////////////////////////

        T getLeft() const     { return m_left; }
        T getTop() const      { return m_top; }
        T getRight() const    { return m_right; }
        T getBottom() const   { return m_bottom; }

        void setLeft (const T left);
        void setTop (const T top);
        void setRight (const T right);
        void setBottom (const T bottom);


        ///////////////////////////
        // Testing functionality //
        ///////////////////////////

        /// <summary> Checks if each point on the rectangle is valid, aka left is not more than right and top is not more than bottom. </summary>
        bool isValid() const;

        /// <summary> Checks if the given rectangle is contained within the current rectangle. </summary>
        bool contains (const Rectangle& other) const;

        /// <summary> Checks to see if any point on the given rectangle intersects with the current rectangle. </summary>
        bool intersects (const Rectangle& other) const;


        //////////////////
        // Calculations //
        //////////////////

        /// <summary> Calculates and returns the width of the rectangle. </summary>
        T width() const     { return m_right - m_left + 1; }

        /// <summary> Calculates and returns the height of the rectangle. </summary>
        T height() const    { return m_bottom - m_top + 1; }

        /// <summary> Calculates the area of the rectangle based on the calculated width and height values. </summary>
        T area() const      { return width() * height(); }

        /// <summary> Calculates the perimeter of the rectangle based on the calculated width and height values. </summary>
        T perimeter() const { return width() * 2 + height() * 2; }


        //////////////////
        // Manipulation //
        //////////////////

        /// <summary> Returns a clipped version of the current rectangle which is inside the given rectangle. </summary>
        Rectangle clipped (const Rectangle& clip) const;

        /// <summary> Will clip the current rectangle so that it is inside of the given rectangle. </summary>
        void clipTo (const Rectangle& other);

        /// <summary> Translates each co-ordinate by the given amount. </summary>
        void translate (const T moveX, const T moveY);

    private:

        /////////////////////////
        // Implementation data //
        /////////////////////////

        T   m_left      { (T) 0 },  //!< The left co-ordinate.
            m_top       { (T) 0 },  //!< The top co-ordinate.
            m_right     { (T) 0 },  //!< The right co-ordinate.
            m_bottom    { (T) 0 };  //!< The bottom co-ordinate.
};


//////////////////
// Constructors //
//////////////////

template <typename T> Rectangle<T>::Rectangle (const T left, const T top, const T right, const T bottom)
    : m_left (left), m_top (top), m_right (right), m_bottom (bottom)
{
}


template <typename T> Rectangle<T>::Rectangle (Rectangle&& move)
{
    *this = std::move (move);
}


template <typename T> Rectangle<T>& Rectangle<T>::operator= (Rectangle&& move)
{
    if (this != &move)
    {
        m_left          = move.m_left;
        m_top           = move.m_top;
        m_right         = move.m_right;
        m_bottom        = move.m_bottom;

        // Reset primitives.
        const T zero    = (T) 0;
        move.m_left     = zero;
        move.m_top      = zero;
        move.m_right    = zero;
        move.m_bottom   = zero;
    }

    return *this;
}


///////////////
// Operators //
///////////////

template <typename T> bool Rectangle<T>::operator== (const Rectangle& rhs) const
{
    return (m_left == rhs.m_left &&
            m_top == rhs.m_top &&
            m_right == rhs.m_right &&
            m_bottom == rhs.m_bottom    );
}


template <typename T> bool Rectangle<T>::operator!= (const Rectangle& rhs) const
{
    return (m_left != rhs.m_left ||
            m_top != rhs.m_top ||
            m_right != rhs.m_right ||
            m_bottom != rhs.m_bottom    );
}


/////////////////////////
// Getters and setters //
/////////////////////////

template <typename T> void Rectangle<T>::setLeft (const T left)
{
    m_left = left;
}


template <typename T> void Rectangle<T>::setTop (const T top)
{
    m_top = top;
}


template <typename T> void Rectangle<T>::setRight (const T right)
{
    m_right = right;
}


template <typename T> void Rectangle<T>::setBottom (const T bottom)
{
    m_bottom = bottom;
}


///////////////////////////
// Testing functionality //
///////////////////////////

template <typename T> bool Rectangle<T>::isValid() const
{
    return (m_left <= m_right && m_top <= m_bottom);
}


template <typename T> bool Rectangle<T>::contains (const Rectangle& other) const
{
    return (m_left <= other.m_left &&
            m_top <= other.m_top &&
            m_right >= other.m_right &&
            m_bottom >= other.m_bottom  );
}


template <typename T> bool Rectangle<T>::intersects (const Rectangle& other) const
{
    return (m_left <= other.m_right &&      // A's left to B's right
            m_top <= other.m_bottom &&      // A's top to B's bottom
            m_right >= other.m_left &&      // A's right to B's left
            m_bottom >= other.m_top     );  // A's bottom to B's top
}


//////////////////
// Manipulation //
//////////////////

template <typename T> Rectangle<T> Rectangle<T>::clipped (const Rectangle& clip) const
{
    // Clip each element, ensuring we maintain valid values.
    const T left    = clip.m_right < m_left ? clip.m_right  : std::max (m_left, clip.m_left),

            top     = clip.m_bottom < m_top ? clip.m_bottom : std::max (m_top, clip.m_top),

            right   = left > m_right        ? left          : std::min (m_right, clip.m_right),

            bottom  = top > m_bottom        ? top           : std::min (m_bottom, clip.m_bottom);

    return { left, top, right, bottom };
}


template <typename T> void Rectangle<T>::clipTo (const Rectangle& clip)
{
    // Clip each element, ensuring we maintain valid values.
    m_left      = clip.m_right < m_left ? clip.m_right  : std::max (m_left, clip.m_left);

    m_top       = clip.m_bottom < m_top ? clip.m_bottom : std::max (m_top, clip.m_top);

    m_right     = m_left > m_right      ? m_left        : std::min (m_right, clip.m_right);

    m_bottom    = m_top > m_bottom      ? m_top         : std::min (m_bottom, clip.m_bottom);
}


template <typename T> void Rectangle<T>::translate (const T moveX, const T moveY)
{
    m_left += moveX;
    m_top += moveY;
    m_right += moveX;
    m_bottom += moveY;
}

#endif
